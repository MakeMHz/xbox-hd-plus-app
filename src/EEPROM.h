#pragma once
#include <stdio.h>
#include <string.h>
#include <memory>

#define EEPROM_PATH "C:\\xboxhd\\eeprom.bin"

enum class OUTPUT_RESOLUTION {
    ORIGINAL = 0,
    C480P,
    C720P,
    C1080I
};

enum class OUTPUT_WIDESCREEN {
    AUTO = 0,
    FORCE,
    NEVER
};

enum class AUDIO_COMP {
    AUTO = 0,
    COMPAT
};

enum class COLORSPACE {
    AUTO = 0,
    YCBCR,
    RGB
};

enum class INTERPOLATION {
    TRUNC = 0,
    BILINEAR
};

enum class INTERPOLATION_X_SCALE {
    TRUNC = 0,
    BILINEAR
};

enum class INTERPOLATION_Y_SCALE {
    TRUNC = 0,
    BILINEAR
};

enum class INTERPOLATION_X_WEIGHT {
    TRUNC = 0,
    BILINEAR
};

enum class INTERPOLATION_Y_WEIGHT {
    TRUNC = 0,
    BILINEAR
};

typedef struct EEPROM_T {
    uint8_t firmware_version[4];
    uint8_t software_version[4];
    uint32_t checksum;
    OUTPUT_RESOLUTION      output_resolution;
    OUTPUT_WIDESCREEN      widescreen;
    AUDIO_COMP             audo_comp;
    COLORSPACE             colorspace;
    INTERPOLATION          interpolation;
    INTERPOLATION_X_SCALE  interpolation_x_scale;
    INTERPOLATION_Y_SCALE  interpolation_y_scale;
    INTERPOLATION_X_WEIGHT interpolation_x_weight;
    INTERPOLATION_Y_WEIGHT interpolation_y_weight;
    uint8_t PADDING[1024];
} __attribute__((packed)) EEPROM_T;

class EEPROM {
    public:
        EEPROM();
        virtual ~EEPROM() { };
        void save();
        EEPROM_T current = { 0 };
    private:
};
