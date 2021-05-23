#include <stdlib.h>
#include <stdio.h>

#include <lwip/debug.h>
#include <lwip/dhcp.h>
#include <lwip/init.h>
#include <lwip/netif.h>
#include <lwip/sys.h>
#include <lwip/tcpip.h>
#include <lwip/timeouts.h>
#include <netif/etharp.h>
#include <pktdrv.h>
#include <xboxkrnl/xboxkrnl.h>
#include <winbase.h>

bool network_init = false;

struct netif nforce_netif;

extern "C" {
err_t nforceif_init(struct netif *netif);
struct netif *g_pnetif;
}

static void packet_timer(void *arg);

sys_sem_t init_complete;
const ip4_addr_t *ip;
static ip4_addr_t ipaddr, netmask, gw;

#define PKT_TMR_INTERVAL 5 /* ms */

static void tcpip_init_done(void *arg)
{
    sys_sem_t *init_complete = (sys_sem_t *)arg;
    sys_sem_signal(init_complete);
}

static void packet_timer(void *arg)
{
    LWIP_UNUSED_ARG(arg);
    Pktdrv_ReceivePackets();
    sys_timeout(PKT_TMR_INTERVAL, packet_timer, NULL);
}

void init_networking()
{
    IP4_ADDR(&gw, 0,0,0,0);
    IP4_ADDR(&ipaddr, 0,0,0,0);
    IP4_ADDR(&netmask, 0,0,0,0);

    // Initialize the TCP/IP stack. Wait for completion
    sys_sem_new(&init_complete, 0);
    tcpip_init(tcpip_init_done, &init_complete);
    sys_sem_wait(&init_complete);
    sys_sem_free(&init_complete);

    g_pnetif = netif_add(&nforce_netif, &ipaddr, &netmask, &gw,
                            NULL, nforceif_init, ethernet_input);
    if(!g_pnetif) {
        debugPrint("netif_add failed\n");
        network_init = false;
        return;
    }

    netif_set_default(g_pnetif);
    netif_set_up(g_pnetif);

    dhcp_start(g_pnetif);

    packet_timer(NULL);

    debugPrint("Waiting for DHCP...\n");
    while(dhcp_supplied_address(g_pnetif) == 0)
        NtYieldExecution();

    network_init = false;
}
