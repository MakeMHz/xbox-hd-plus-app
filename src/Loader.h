#pragma once

enum BOOT_OPTION {
    BOOT_NONE = 0,
    BOOT_LOAD_APP,
    BOOT_CLEAR_EEPROM,
    BOOT_LOAD_TEST,
    BOOT_FORCE_AUTO,
    BOOT_FORCE_480P,
    BOOT_FORCE_720P,
    BOOT_FORCE_1080I
};

int main_loader(void);
