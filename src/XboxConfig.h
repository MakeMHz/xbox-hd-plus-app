#pragma once
#include <windows.h>

#define XC_VIDEO_FLAGS 8
#define XC_AUDIO       9

#define XC_VIDEO_FLAGS_WIDESCREEN   (0x00000001 << 16)
#define XC_VIDEO_FLAGS_HDTV_720p    (0x00000002 << 16)
#define XC_VIDEO_FLAGS_HDTV_1080i   (0x00000004 << 16)
#define XC_VIDEO_FLAGS_HDTV_480p    (0x00000008 << 16)
#define XC_VIDEO_FLAGS_LETTERBOX    (0x00000010 << 16)
#define XC_VIDEO_FLAGS_PAL_60Hz     (0x00000040 << 16)

#define XC_AUDIO_FLAGS_STEREO       0x00000000
#define XC_AUDIO_FLAGS_MONO         0x00000001
#define XC_AUDIO_FLAGS_SURROUND     0x00000002
#define XC_AUDIO_FLAGS_ENABLE_AC3   0x00010000
#define XC_AUDIO_FLAGS_ENABLE_DTS   0x00020000

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
    void SetAudioMode(ULONG flag);
    void SetAudioAC3(bool enable);
    void SetAudioDTS(bool enable);

    // Generic class functions
    void Save();

    // Public class variables
    ULONG VideoFlags;
    ULONG AudioFlags;
private:
    ULONG VideoFlagsType;
    ULONG AudioFlagsType;
};
