#include <stdio.h>
#include <string.h>
#include <fstream>

#include "EEPROM.h"
#include "Version.h"

EEPROM::EEPROM() {
    bool is_eeprom_valid = true;

    memset(&current, 0, sizeof(EEPROM_T));

    std::ifstream file(EEPROM_PATH, std::ifstream::binary);
    if(file.is_open())
        file.read((char *)&current, sizeof(EEPROM_T));
    file.close();

    //
    semver_t eeprom_software_version = { current.software_version[0], current.software_version[1], current.software_version[2], NULL, NULL };
    semver_t eeprom_firmware_version = { current.firmware_version[0], current.firmware_version[1], current.firmware_version[2], NULL, NULL };

    // Check software version. (Mismatch should be treated as invalid in all cases)
    if(semver_compare(eeprom_software_version, current_version) != 0)
        is_eeprom_valid = false;

    // Check target firmware version. (Mismatch should be treated as invalid in all cases)
    if(semver_compare(eeprom_firmware_version, target_firmware_version) != 0)
        is_eeprom_valid = false;

    // If eeprom is invalid then clear and set default values
    if(!is_eeprom_valid) {
        memset(&current, 0, sizeof(EEPROM_T));

        // Set software version
        current.software_version[0] = (current_version.major & 0xFF);
        current.software_version[1] = (current_version.minor & 0xFF);
        current.software_version[2] = (current_version.patch & 0xFF);

        // Set target firmware version
        current.firmware_version[0] = (target_firmware_version.major & 0xFF);
        current.firmware_version[1] = (target_firmware_version.minor & 0xFF);
        current.firmware_version[2] = (target_firmware_version.patch & 0xFF);

        // Save updated EEPROM
        save();
    }
}

void EEPROM::save() {
    std::ofstream file(EEPROM_PATH);

    // Calculate checksum

    file.write((char *)&current, sizeof(EEPROM_T));
    file.close();
}
