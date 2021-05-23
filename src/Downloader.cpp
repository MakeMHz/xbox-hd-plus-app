#include <stdio.h>
#include <iostream>
#include <lwip/apps/http_client.h>

#include "Downloader.h"

static void ResultEventCB(void *arg, httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err)
{
    Downloader* downloader = static_cast<Downloader *>(arg);
    downloader->result_fn(httpc_result, rx_content_len, srv_res, err);
}

static err_t HeadersEventCB(httpc_state_t *connection, void *arg, struct pbuf *hdr, u16_t hdr_len, u32_t content_len)
{
    Downloader* downloader = static_cast<Downloader *>(arg);
    return downloader->headers_done_fn(connection, hdr, hdr_len, content_len);
}

Downloader::Downloader(const char *server_name, uint16_t port, const char *uri, const char *dest)
{
    RtlZeroMemory(&settings, sizeof(settings));
    settings.result_fn = ResultEventCB;
    settings.headers_done_fn = HeadersEventCB;

    //
    error = httpc_get_file_dns_to_disk(server_name, 80, uri, &settings, static_cast<void *>(this), dest, &connection);

    return;
}

uint8_t Downloader::getProgress()
{
    return progress;
}

uint16_t Downloader::getStatus()
{
    return (error == ERR_OK) ? 200 : 400;
}

void Downloader::result_fn(httpc_result_t httpc_result, u32_t rx_content_len, u32_t srv_res, err_t err) {
    progress = 100;
}

err_t Downloader::headers_done_fn(httpc_state_t *connection, struct pbuf *hdr, u16_t hdr_len, u32_t content_len) {
    return ERR_OK;
}
