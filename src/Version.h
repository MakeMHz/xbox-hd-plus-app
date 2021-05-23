#pragma once
#include <semver.h>

extern semver_t current_version;

bool findKernelPatchVersion(uint8_t *version);
