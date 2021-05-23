#pragma once
#include <stdint.h>

int main_app(void);

extern uint8_t load_scene;

extern uint8_t firmware_version[3];
extern uint8_t kernel_patch_version[3];

void drawFirmwareVersion();
void drawSoftwareVersion();
void drawPatchVersion();
