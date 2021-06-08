#include <stdio.h>
#include <xboxkrnl/xboxkrnl.h>
#include "App.h"
#include "Loader.h"

#define KPATCH_BOOT 0x0000B007

typedef struct _LAUNCH_DATA {
    uint32_t mode;
} LAUNCH_DATA, *PLAUNCH_DATA;

int main(void)
{
    PLAUNCH_DATA LaunchData;

    if(LaunchDataPage != NULL) {
        LaunchData = (PLAUNCH_DATA)&(LaunchDataPage->LaunchData[0]);

        if(LaunchData->mode == KPATCH_BOOT) {
            main_loader();
            return 0;
        }
    }

    main_app();
    return 0;
}
