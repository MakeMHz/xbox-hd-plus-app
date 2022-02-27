#pragma once
#include <windows.h>

#define XC_VIDEO_FLAGS 8

#define XC_VIDEO_FLAGS_WIDESCREEN   (0x00000001 << 16)
#define XC_VIDEO_FLAGS_HDTV_720p    (0x00000002 << 16)
#define XC_VIDEO_FLAGS_HDTV_1080i   (0x00000004 << 16)
#define XC_VIDEO_FLAGS_HDTV_480p    (0x00000008 << 16)
#define XC_VIDEO_FLAGS_LETTERBOX    (0x00000010 << 16)
#define XC_VIDEO_FLAGS_PAL_60Hz     (0x00000040 << 16)

class XboxConfig {
public:
    XboxConfig();
    virtual ~XboxConfig() { };

    // Video Settings
    void SetVideo480p(bool enable);
    void SetVideo720p(bool enable);
    void SetVideo1080i(bool enable);

    void SetVideoAspectRatio(ULONG flag);

    // Audio settings

    // Generic class functions
    void Save();

    // Public class variables
    ULONG VideoFlags;
private:
    ULONG VideoFlagsType;
};
