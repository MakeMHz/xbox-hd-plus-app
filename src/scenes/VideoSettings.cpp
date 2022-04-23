#include <stdio.h>
#include <string.h>
#include <lvgl.h>

#include "lvgl-extra/ButtonGroup.h"
#include "lvgl-extra/SwitchLabel.h"
#include "lvgl-extra/WarpObject.h"

#include "App.h"
#include "scenes/VideoSettings.h"

static const char * optionResolution[] = { "480P", "720P", "" };
static const char * optionWidescreen[] = { "Auto", "Force", "Never", "" };

static const char helpTargetResolution[] =
    "Target resolution controls the minimum output resolution.\n\n"
    "Video output will be upscaled if the Xbox's framebuffer is smaller than the set target resolution. \n\n"
    "Original will output a 1:1 video signal of the active framebuffer with no scaling.\n\n"
    "This option does not limit the output resolution. (For example, software can still run at 1080i, if the "
    "'Video Mode' option is enabled for that resolution, and 'Target Resolution' is set to 720p)";

static const char helpWidescreen[] =
    "The 4:3 Widescreen option controls how 640x480 content should be displayed.\n\n"
    "Auto:  Automatically stretch anamorphic 640x480 to 16:9.\n\n"
    "Force: Always stretch 640x480 content to 16:9.\n\n"
    "Never: Never stretch 640x480 content to 16:9.";

static const char helpPrescaleCorrection[] =
    "The prescale correction option will scale the video output of 640x480 4:3 content framed in 480p "
    "so that the final displayed image has a true 4:3 aspect ratio.\n\n"
    "This due to how most displays unpack\n4:3 480p content and ignore AVI Infoframe bar data.\n\n"
    "This option only has an affect, and is only needed, when the output target is set to 480p.";

static const char helpDisable480p16X9[] =
    "Retail NTSC titles are allowed to request 480P without explictly stating if the desired "
    "picture aspect ratio is 16:9 (both for 720x480 and 640x480).\n\n"
    "Enabling this option is the best assumption for most retail games and will automaticly use "
    "anamphoric widescreen in those cases (with respects to the '4:3 Widescreen' option).";

static void ButtonEventHandler(lv_obj_t * obj, lv_event_t event) {
    VideoSettings* scene = static_cast<VideoSettings *>(obj->user_data);
    scene->OnObjectEvent(obj, event);
}

VideoSettings::VideoSettings()
{
    // Set the size and position of our scene
    lv_obj_set_size(cont, 320, 420);
    lv_obj_set_pos(cont, 380, 20);

    lv_cont_set_layout(cont, LV_LAYOUT_COLUMN_LEFT);

    // Create help text panel
    contHelp = lv_cont_create(screen, NULL);

    lv_obj_set_style_local_bg_opa(contHelp, LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_90);
    lv_obj_set_style_local_border_width(contHelp, LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, 0);

    lv_obj_set_size(contHelp, 330, 340);
    lv_obj_set_pos(contHelp, 20, 20);
    lv_obj_set_hidden(contHelp, true);

    contHelpLabel = lv_label_create(contHelp, NULL);
    lv_label_set_long_mode(contHelpLabel, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(contHelpLabel, 300);
    lv_obj_align(contHelpLabel, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);

    //
    buttonMatrix[0] = new ButtonGroup(cont, group, "Target Resolution (Upscale)", optionResolution, (uint8_t *)&gEEPROM->current.output_resolution);
    buttonMatrix[1] = new ButtonGroup(cont, group, "4:3 Widescreen", optionWidescreen, (uint8_t *)&gEEPROM->current.widescreen);

    //
    btnPrescaleCorrection = new SwitchLabel(cont, group, "Pre-scale 480p Correction", !gEEPROM->current.prescale_fix_disable);
    btnDisable480p16X9    = new SwitchLabel(cont, group, "Assume 16:9 480p(HD)", !gEEPROM->current.disable_480P_16_9);

    // Register a callbacks
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), buttonMatrix[0]->buttons);
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), buttonMatrix[1]->buttons);
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), btnPrescaleCorrection->lv_switch);
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), btnDisable480p16X9->lv_switch);

    // Draw back button in default location
    DrawBackButton();

    // Update helper text
    UpdateHelperText();

    // Reset warp so that the newly focused object is set as current
    WarpObjectReset(group);
}

VideoSettings::~VideoSettings(void)
{
    // Store values now since we can not trust lvgl's events to give us the correct states
    gEEPROM->current.output_resolution = (uint8_t)lv_btnmatrix_get_btn_ctrl(buttonMatrix[0]->buttons, 1, LV_BTNMATRIX_CTRL_CHECK_STATE);
    if((uint8_t)lv_btnmatrix_get_btn_ctrl(buttonMatrix[1]->buttons, 1, LV_BTNMATRIX_CTRL_CHECK_STATE)) {
        gEEPROM->current.widescreen = 1;
    } else if((uint8_t)lv_btnmatrix_get_btn_ctrl(buttonMatrix[1]->buttons, 2, LV_BTNMATRIX_CTRL_CHECK_STATE)) {
        gEEPROM->current.widescreen = 2;
    } else {
        gEEPROM->current.widescreen = 0;
    }

    gEEPROM->current.prescale_fix_disable = !(uint8_t)lv_switch_get_state(btnPrescaleCorrection->lv_switch);
    gEEPROM->current.disable_480P_16_9    = !(uint8_t)lv_switch_get_state(btnDisable480p16X9->lv_switch);

    // Save EEPROM
    gEEPROM->save();

    // TODO: Clean up all of the other objects.
    lv_obj_del(screen);
}

void VideoSettings::UpdateHelperText() {
    lv_obj_t *focus = lv_group_get_focused(group);

    if(focus == buttonMatrix[0]->buttons) {
        lv_label_set_text(contHelpLabel, helpTargetResolution);
        lv_obj_set_hidden(contHelp, false);
    } else if(focus == buttonMatrix[1]->buttons) {
        lv_label_set_text(contHelpLabel, helpWidescreen);
        lv_obj_set_hidden(contHelp, false);
    } else if(focus == btnPrescaleCorrection->lv_switch) {
        lv_label_set_text(contHelpLabel, helpPrescaleCorrection);
        lv_obj_set_hidden(contHelp, false);
    } else if(focus == btnDisable480p16X9->lv_switch) {
        lv_label_set_text(contHelpLabel, helpDisable480p16X9);
        lv_obj_set_hidden(contHelp, false);
    } else
        lv_obj_set_hidden(contHelp, true);
};

void VideoSettings::OnObjectEvent(lv_obj_t* obj, lv_event_t event)
{
    // Preform warp check on event
    if(WarpObjectOnEvent(obj, event, group)) { return; }

    // HACK: Escape now if the event was triggered after exit (lvgl event handler can trigger after destructor call)
    if(load_scene != SCENE::VIDEO_SETTINGS)
        return;

    if(event == LV_EVENT_FOCUSED)
        UpdateHelperText();

    if(event == LV_EVENT_KEY) {
        uint32_t event_key = *(uint32_t *)lv_event_get_data();

        if(event_key == LV_KEY_ESC) {
            // Return to previous scene
            load_scene = SCENE::ROOT;
        }
    }
}
