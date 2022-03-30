#include <stdint.h>
#include <lvgl.h>
#include <lv_sdl_drv_display.h>
#include <lv_sdl_drv_input.h>
#include <hal/video.h>
#include <hal/xbox.h>
#include <nxdk/mount.h>
#include <nxdk/path.h>
#include <xboxkrnl/xboxkrnl.h>

#include "App.h"
#include "Assets.h"
#include "EEPROM.h"
#include "FatalError.h"
#include "Network.h"
#include "Scene.h"
#include "System.h"
#include "Update.h"
#include "Version.h"

#include "scenes/AdvanceSettings.h"
#include "scenes/ColorCorrection.h"
#include "scenes/Credits.h"
#include "scenes/InterpolationSettings.h"
#include "scenes/OverscanCorrection.h"
#include "scenes/Root.h"
#include "scenes/SelfTest.h"
#include "scenes/SystemAudioSettings.h"
#include "scenes/SystemSettings.h"
#include "scenes/UserManual.h"
#include "scenes/VideoSettings.h"

#define printf(fmt, ...) debugPrint(fmt, __VA_ARGS__)

static int width  = 720;
static int height = 480;

uint8_t current_scene = 0;
uint8_t load_scene = 0;

EEPROM *gEEPROM;
Update *update;

int main_app(void)
{
    if(!XVideoSetMode(width, height, LV_COLOR_DEPTH, REFRESH_DEFAULT)) {
        HalReturnToFirmware(HalRebootRoutine);
        return 0;
    }

    // Mount the C partition
    nxMountDrive('C', "\\Device\\Harddisk0\\Partition2\\");

    lv_init();
    lv_sdl_init_display("XboxHDMI", width, height);
    lv_sdl_init_input();

    lv_disp_set_bg_opa(NULL, LV_OPA_TRANSP);
    lv_obj_set_style_local_bg_opa(lv_scr_act(), LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);

    lv_obj_t * wp = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(wp, &background_left);
    lv_obj_set_pos(wp, 0, 0);
    lv_obj_set_width(wp, LV_HOR_RES);

    // Verify that we launched from C://xboxhd//default.xbe
    // TODO: If it's from the DVD drive then prompt to install? Or at least show an warning/error.
    checkInstallDir();
    checkInstallDirPatch();

    // System checks
    checkSystemRegion();

    drawFirmwareVersion();
    drawSoftwareVersion();
    drawPatchVersion();

    checkPatchVersion();

    // Check if firmware needs to be updated
    checkFirmwareV2();
    checkFirmwareLastest();

    // TODO: Verify patch.bin is loaded

    //
    gEEPROM = new EEPROM();
    update = new Update();

    Scene *currentScene = new SceneRoot();

    // Main render loop
    while(!get_quit_event())
    {
      if(load_scene != current_scene) {
            delete currentScene;

        switch(load_scene) {
            case SCENE::ROOT:
                currentScene = new SceneRoot();
                current_scene = load_scene;
                break;
            case SCENE::SYSTEM_SETTINGS:
                currentScene = new SystemSettings();
                current_scene = load_scene;
                break;
            case SCENE::SYSTEM_AUDIO_SETTINGS:
                currentScene = new SystemAudioSettings();
                current_scene = load_scene;
                break;
            case SCENE::ADVANCED_SETTINGS:
                currentScene = new AdvanceSettings();
                current_scene = load_scene;
                break;
            case SCENE::USER_MANUAL:
                currentScene = new UserManual();
                current_scene = load_scene;
                break;
            case SCENE::COLOR_CORRECTION:
                currentScene = new ColorCorrection();
                current_scene = load_scene;
                break;
            case SCENE::CREDITS:
                currentScene = new Credits();
                current_scene = load_scene;
                break;
            case SCENE::INTERPOLATION_SETTINGS:
                currentScene = new InterpolationSettings();
                current_scene = load_scene;
                break;
            case SCENE::OVERSCAN_CORRECTION:
                currentScene = new OverscanCorrection();
                current_scene = load_scene;
                break;
            case SCENE::SELF_TEST:
                currentScene = new SceneSelfTest();
                current_scene = load_scene;
                break;
            case SCENE::VIDEO_SETTINGS:
                currentScene = new VideoSettings();
                current_scene = load_scene;
                break;
            default:
                currentScene = new SceneRoot();
                current_scene = load_scene;
                break;
            }
        }

        lv_task_handler();
        Sleep(15);
    }

    lv_sdl_deinit_input();
    lv_sdl_deinit_display();
    lv_deinit();

    // Reboot Xbox so kpatch has a chance to reload
    HalReturnToFirmware(HalRebootRoutine);

    return 0;
}

uint8_t firmware_version[3] = { 0 };
lv_obj_t *label_firmware;
void drawFirmwareVersion()
{
    uint8_t firmware_version[3];

    label_firmware = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(label_firmware, 30, 420);

    //
    if(getFirmwareVersion(firmware_version)) {
        lv_label_set_text_fmt(label_firmware, "Firmware Version: %u.%u.%u",
            firmware_version[0], firmware_version[1], firmware_version[2]);
    } else {
        lv_label_set_text(label_firmware, "XboxHD+ not detected!");
    }
}

lv_obj_t *label_software;
void drawSoftwareVersion()
{
    label_software = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(label_software, 30, 380);

    lv_label_set_text_fmt(label_software, "Software Version: %d.%d.%d",
        current_version.major, current_version.minor, current_version.patch);
}

uint8_t kernel_patch_version[3] = { 0 };
lv_obj_t *label_patch;
void drawPatchVersion()
{
    label_patch = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(label_patch, 30, 400);

    if(findKernelPatchVersion(kernel_patch_version)) {
        lv_label_set_text_fmt(label_patch, "BIOS Patch Version: %u.%u.%u",
            kernel_patch_version[0], kernel_patch_version[1], kernel_patch_version[2]);
    } else {
        if(findKernelPatchLoader())
            lv_label_set_text(label_patch, "Kernel patch loader detected");
        else
            lv_label_set_text(label_patch, "Kernel patch not detected!");
    }
}

void checkPatchVersion() {
    static const char *btns[] = { "Reboot", "" };

    if(kernel_patch_version[0] == 0)
        return;

    semver_t current_patch_version = { kernel_patch_version[0], kernel_patch_version[1], kernel_patch_version[2], NULL, NULL };
    semver_t target_patch_version  = { 2, 0, 0, NULL, NULL };

    if(semver_satisfies(current_patch_version, target_patch_version, "<")) {
        // Register input device
        lv_indev_t *sdl_indev = lv_indev_get_next(NULL);
        lv_group_t *group = lv_group_create();
        if(sdl_indev != NULL)
            lv_indev_set_group(sdl_indev, group);


        lv_obj_t * mbox1 = lv_msgbox_create(lv_scr_act(), NULL);
        lv_msgbox_set_text_fmt(mbox1,
            "Kernel patch version (v%d.%d.%d) incompatible.\nIt appears your currently running a none XboxHD+ patched BIOS.\nPlease verify that your BIOS is patched correctly.\n(If you have a XboxHDMI board then make sure to follow the upgrade guide!)",
            current_patch_version.major, current_patch_version.minor, current_patch_version.patch
        );

        lv_msgbox_add_btns(mbox1, btns);
        lv_obj_set_width(mbox1, 400);
        lv_group_add_obj(group, mbox1);
        lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0);

        while(!get_quit_event()) {
            if(lv_msgbox_get_active_btn(mbox1)) {
                XReboot();
            }

            lv_task_handler();
            Sleep(15);
        }
    }
}

void checkFirmwareV2() {
    static const char *btns[] = { "Update", "" };

    uint8_t firmware_version[3];
    if(!getFirmwareVersion(firmware_version))
        return;

    semver_t current_firmware_version = { firmware_version[0], firmware_version[1], firmware_version[2], NULL, NULL };
    semver_t target_firmware_version  = { 2, 0, 0, NULL, NULL };

    if(semver_satisfies(current_firmware_version, target_firmware_version, "<")) {
        // Register input device
        lv_indev_t *sdl_indev = lv_indev_get_next(NULL);
        lv_group_t *group = lv_group_create();
        if(sdl_indev != NULL)
            lv_indev_set_group(sdl_indev, group);


        lv_obj_t * mbox1 = lv_msgbox_create(lv_scr_act(), NULL);
        lv_msgbox_set_text_fmt(mbox1,
            "A firmware update is required to continue.\n(v%d.%d.%d to v%d.%d.%d)",
            current_firmware_version.major, current_firmware_version.minor, current_firmware_version.patch,
            target_firmware_version.major, target_firmware_version.minor, target_firmware_version.patch
        );

        lv_msgbox_add_btns(mbox1, btns);
        lv_obj_set_width(mbox1, 400);
        lv_group_add_obj(group, mbox1);
        lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0);

        while(!get_quit_event()) {
            if(lv_msgbox_get_active_btn(mbox1)) {
                XLaunchXBE("C:\\xboxhd\\upgrade.xbe");
            }

            lv_task_handler();
            Sleep(15);
        }
    }
}

void checkFirmwareLastest() {
    static const char *btns[] = { "Update", "" };

    uint8_t firmware_version[3];
    if(!getFirmwareVersion(firmware_version))
        return;

    semver_t current_firmware_version = { firmware_version[0], firmware_version[1], firmware_version[2], NULL, NULL };
    semver_t target_firmware_version  = { 0, 0, 0, NULL, NULL };

    FILE *fp = fopen("C:\\xboxhd\\firmware.xbe", "rb");
    uint8_t *firmware = NULL;
    long fsize;

    if(!fp)
        return;

    //
    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    rewind(fp);

    //
    firmware = (uint8_t *)malloc(fsize + 1);
    fread(firmware, 1, fsize, fp);
    fclose(fp);

    //
    for(uint32_t offset = 0; offset < (fsize - 100); offset++) {
        char tag[] = "FMv";

        if(memcmp(tag, firmware + offset, sizeof(tag) - 1) == 0) {
            target_firmware_version = { (firmware + offset)[3], (firmware + offset)[4], (firmware + offset)[5], NULL, NULL };
            break;
        }
    }

    //
    free(firmware);

    if(semver_compare(current_firmware_version, target_firmware_version) == -1) {
        // Register input device
        lv_indev_t *sdl_indev = lv_indev_get_next(NULL);
        lv_group_t *group = lv_group_create();
        if(sdl_indev != NULL)
            lv_indev_set_group(sdl_indev, group);

        lv_obj_t * mbox1 = lv_msgbox_create(lv_scr_act(), NULL);
        lv_msgbox_set_text_fmt(mbox1,
            "A firmware update is required to continue.\n(v%d.%d.%d to v%d.%d.%d)",
            current_firmware_version.major, current_firmware_version.minor, current_firmware_version.patch,
            target_firmware_version.major, target_firmware_version.minor, target_firmware_version.patch
        );

        lv_msgbox_add_btns(mbox1, btns);
        lv_obj_set_width(mbox1, 400);
        lv_group_add_obj(group, mbox1);
        lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0);

        while(!get_quit_event()) {
            if(lv_msgbox_get_active_btn(mbox1)) {
                XLaunchXBE("C:\\xboxhd\\firmware.xbe");
            }

            lv_task_handler();
            Sleep(15);
        }
    }
}

void checkInstallDir() {
    char allowedPath[] = "\\Device\\Harddisk0\\Partition2\\xboxhd\\default.xbe";
    char launchPath[MAX_PATH];
    nxGetCurrentXbeNtPath(launchPath);

    if(strcmp(launchPath, allowedPath) != 0)
        DisplayFatalError("ERROR: The XboxHD app must be installed in\nC:\\xboxhd");
}

void checkInstallDirPatch() {
    FILE *file = fopen("D:\\kp.bin", "r");
    if(!file)
        DisplayFatalError("ERROR: XboxHD+ installation corrupt.\n(Missing File: kp.bin)\n\nPlease verify/reinstall the XboxHD+ files.");
    fclose(file);
}

void checkSystemRegion() {
    ULONG type, value;

    // AV Region
    ExQueryNonVolatileSetting(259, &type,  &value, sizeof(value), NULL);
    if(((value & 0xFF00) >> 8) != 1)
        DisplayFatalError("ERROR: Invalid system AV region. Please change AV region to NTSC before continuing.");

    // Game region
    ExQueryNonVolatileSetting(260, &type,  &value, sizeof(value), NULL);
    if(value != 1)
        DisplayFatalError("ERROR: Invalid system game region. Please change game region to NTSC before continuing.");
}
