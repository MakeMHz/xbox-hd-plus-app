#include <stdint.h>
#include <string.h>
#include <semver.h>
#include <xboxkrnl/xboxkrnl.h>

semver_t current_version = { 2, 0, 0 };

// The HD+ firmware version that this copy of the software supports.
// Used before executing commands on the MCU and for the 'EEPROM'
semver_t target_firmware_version = { 2, 0, 0 };

bool findKernelPatchVersion(uint8_t *version) {
    char tag[] = "NPkv";
    char *findStartAddress = ((char *)&AvGetSavedDataAddress) - 0x00002000;

    for(uint16_t offset = 0; offset < 0x00004000; offset++) {
        if(memcmp(tag, findStartAddress + offset, sizeof(tag) - 1) == 0) {
        version[0] = (findStartAddress + offset)[6];
        version[1] = (findStartAddress + offset)[7];
        version[2] = (findStartAddress + offset)[8];

        return true;
        }
    }

    return false;
}
