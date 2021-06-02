#include <stdint.h>
#include <lvgl.h>
#include <lv_sdl_drv_display.h>
#include <lv_sdl_drv_input.h>
#include <hal/video.h>
#include <nxdk/mount.h>

#include "App.h"
#include "Assets.h"
#include "EEPROM.h"
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
    XVideoSetMode(width, height, LV_COLOR_DEPTH, REFRESH_DEFAULT);

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

    drawFirmwareVersion();
    drawSoftwareVersion();
    drawPatchVersion();

    // TODO: Verify patch.bin is loaded

    // TODO: Verify that we launched from C://xboxhd//default.xbe
    // If it's from the DVD drive then prompt to install? Or at least show an warning/error.

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
            case SCENE::ADVANCED_SETTINGS:
                currentScene = new AdvanceSettings();
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

    return 0;
}

uint8_t firmware_version[3] = { 0 };
lv_obj_t *label_firmware;
void drawFirmwareVersion()
{
    ULONG smbus_read;

    label_firmware = lv_label_create(lv_scr_act(), NULL);
    lv_obj_set_pos(label_firmware, 30, 420);

    //
    if(HalReadSMBusValue(I2C_HDMI_ADRESS, I2C_FIRMWARE_VERSION + 0, false, &smbus_read) != 0) {
        lv_label_set_text(label_firmware, "XboxHD+ not detected!");
    } else {
        firmware_version[0] = (uint8_t)smbus_read;

        HalReadSMBusValue(I2C_HDMI_ADRESS, I2C_FIRMWARE_VERSION + 1, false, &smbus_read);
        firmware_version[1] = (uint8_t)smbus_read;

        HalReadSMBusValue(I2C_HDMI_ADRESS, I2C_FIRMWARE_VERSION + 2, false, &smbus_read);
        firmware_version[2] = (uint8_t)smbus_read;

        // Firmware 1.0.0 will incorrectly report 0.0.0, so let's fix that.
        if(firmware_version[0] == 0 && firmware_version[1] == 0 && firmware_version[2] == 0) {
            firmware_version[0] = 1;
        }

        lv_label_set_text_fmt(label_firmware, "Firmware Version: %u.%u.%u",
            firmware_version[0], firmware_version[1], firmware_version[2]);
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
        lv_label_set_text(label_patch, "Kernel patch not detected!");
    }
}
