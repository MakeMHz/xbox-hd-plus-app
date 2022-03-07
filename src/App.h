#pragma once
#include <stdint.h>
#include "EEPROM.h"

int main_app(void);

extern EEPROM *gEEPROM;

extern uint8_t load_scene;

extern uint8_t firmware_version[3];
extern uint8_t kernel_patch_version[3];

void drawFirmwareVersion();
void drawSoftwareVersion();
void drawPatchVersion();

void checkPatchVersion();
void checkFirmwareV2();
void checkFirmwareLastest();

void checkInstallDir();
void checkInstallDirPatch();

void checkSystemRegion();
