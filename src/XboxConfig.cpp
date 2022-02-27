#include <xboxkrnl/xboxkrnl.h>
#include <windows.h>

#include "XboxConfig.h"

XboxConfig::XboxConfig() {
    ULONG ResultLength;
    ExQueryNonVolatileSetting(XC_VIDEO_FLAGS, &VideoFlagsType, &VideoFlags, sizeof(VideoFlags), &ResultLength);
}

void XboxConfig::SetVideo480p(bool enable) {
    VideoFlags = (VideoFlags & ~XC_VIDEO_FLAGS_HDTV_480p) + (enable ? XC_VIDEO_FLAGS_HDTV_480p : 0);
}

void XboxConfig::SetVideo720p(bool enable) {
    VideoFlags = (VideoFlags & ~XC_VIDEO_FLAGS_HDTV_720p) + (enable ? XC_VIDEO_FLAGS_HDTV_720p : 0);
}

void XboxConfig::SetVideo1080i(bool enable) {
    VideoFlags = (VideoFlags & ~XC_VIDEO_FLAGS_HDTV_1080i) + (enable ? XC_VIDEO_FLAGS_HDTV_1080i : 0);
}

void XboxConfig::SetVideoAspectRatio(ULONG flag) {
    VideoFlags = (VideoFlags & ~(XC_VIDEO_FLAGS_WIDESCREEN | XC_VIDEO_FLAGS_LETTERBOX)) + flag;
}

void XboxConfig::Save() {
    ULONG ResultLength;
    ExSaveNonVolatileSetting(XC_VIDEO_FLAGS, VideoFlagsType, &VideoFlags, sizeof(VideoFlags));
}
