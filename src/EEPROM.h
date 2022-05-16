#pragma once
#include <stdio.h>
#include <string.h>
#include <memory>

#define EEPROM_PATH "C:\\xboxhd\\eeprom.bin"

typedef struct EEPROM_T {
    uint8_t  firmware_version[4];
    uint8_t  software_version[4];
    uint32_t checksum;
    uint8_t  output_resolution;
    uint8_t  widescreen;
    uint8_t  colorspace;
    uint8_t  interpolation;
    uint8_t  interpolation_x_scale;
    uint8_t  interpolation_y_scale;
    uint8_t  interpolation_x_weight;
    uint8_t  interpolation_y_weight;
    uint8_t  auto_video_blank;
    uint8_t  auto_region_switch;
    uint8_t  prescale_fix_disable;
    uint8_t  disable_480P_16_9;

    // AVI Infoframe Overrides
    uint8_t   avi_info_override;
    uint8_t   picture_aspect_ratio;
    uint8_t   active_format_aspect_ratio;
    uint8_t   active_format_status;
    uint8_t   bar_info_present;
    uint16_t  bar_info_etb;
    uint16_t  bar_info_sbb;
    uint16_t  bar_info_elb;
    uint16_t  bar_info_srb;

    // Pixel Clock Delay Override
    uint8_t   pixel_clock_delay_override;
    uint8_t   pixel_clock_delay;
    uint8_t   pixel_clock_delay_neg;

    uint8_t  PADDING[1024];
} __attribute__((packed)) EEPROM_T;

class EEPROM {
    public:
        EEPROM();
        virtual ~EEPROM() { };
        void clear();
        void save();
        EEPROM_T current = { 0 };
    private:
};
