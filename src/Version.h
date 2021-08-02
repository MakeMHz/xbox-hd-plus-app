#pragma once
#include <semver.h>

extern semver_t current_version;
extern semver_t target_firmware_version;

bool findKernelPatchVersion(uint8_t *version);
bool findKernelPatchLoader();
bool getFirmwareVersion(uint8_t *firmware_version);
