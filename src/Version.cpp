#include <stdint.h>
#include <string.h>
#include <semver.h>
#include <xboxkrnl/xboxkrnl.h>
#include "System.h"

// Version of this software
semver_t current_version = { 2, 3, 0 };

// The HD+ firmware version that this copy of the software supports.
// Used before executing commands on the MCU and for the 'EEPROM'
semver_t target_firmware_version = { 2, 3, 0 };

bool findKernelPatchVersion(uint8_t *version) {
    char tag[] = "HDMIkv";
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

bool findKernelPatchLoader() {
    char tag[] = "kp.bin";
    char *findStartAddress = ((char *)0x80030E00);

    for(uint16_t offset = 0; offset < 0x00000100; offset++) {
        if(memcmp(tag, findStartAddress + offset, sizeof(tag) - 1) == 0)
            return true;
    }

    return false;
}

bool getFirmwareVersion(uint8_t *firmware_version) {
    ULONG smbus_read;

    // Check for XboxHDMI/HD+
    if(HalReadSMBusValue(I2C_HDMI_ADDRESS, I2C_FIRMWARE_VERSION + 0, 0, &smbus_read) != 0) {
        return false;
    }

    firmware_version[0] = (uint8_t)smbus_read;

    HalReadSMBusValue(I2C_HDMI_ADDRESS, I2C_FIRMWARE_VERSION + 1, 0, &smbus_read);
    firmware_version[1] = (uint8_t)smbus_read;

    HalReadSMBusValue(I2C_HDMI_ADDRESS, I2C_FIRMWARE_VERSION + 2, 0, &smbus_read);
    firmware_version[2] = (uint8_t)smbus_read;

    // Firmware 1.0.0 will incorrectly report 0.0.0, so let's fix that.
    if(firmware_version[0] == 0 && firmware_version[1] == 0 && firmware_version[2] == 0) {
        firmware_version[0] = 1;
        firmware_version[1] = 0;
        firmware_version[2] = 0;
    }

    return true;
}
