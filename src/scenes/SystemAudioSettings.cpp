#include <stdio.h>
#include <string.h>
#include <lvgl.h>
#include <windows.h>

#include "App.h"
#include "Scene.h"
#include "XboxConfig.h"

#include "lvgl-extra/ButtonGroup.h"
#include "lvgl-extra/WarpObject.h"

#include "scenes/SystemAudioSettings.h"

static const char * optionAudioMode[] = { "Mono", "Stereo", "Surround", "" };
static const char * optionDolby[]     = { "Disabled", "Enabled", "" };
static const char * optionDTS[]       = { "Disabled", "Enabled", "" };

static const char helpAudioSettings[] =
    "The audio mode options controls how software on the Xbox can output audio.\n\n"
    "Surround sound will only work correctly if the receiving device supports the enabled audio codec.\n\n"
    "* These are the same settings from the Microsoft dashboard.";

static void ButtonEventHandler(lv_obj_t * obj, lv_event_t event) {
    SystemAudioSettings* scene = static_cast<SystemAudioSettings *>(obj->user_data);
    scene->OnObjectEvent(obj, event);
}

SystemAudioSettings::SystemAudioSettings()
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

    contHelpLabel = lv_label_create(contHelp, NULL);
    lv_label_set_long_mode(contHelpLabel, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(contHelpLabel, 300);
    lv_obj_align(contHelpLabel, NULL, LV_ALIGN_IN_TOP_MID, 0, 20);
    lv_label_set_text(contHelpLabel, helpAudioSettings);

    //
    buttonMatrix[0] = new ButtonGroup(cont, group, "Audio Mode", optionAudioMode, NULL);

    //
    buttonMatrix[1] = new ButtonGroup(cont, group, "Dolby Digital (AC3)", optionDolby, NULL);

    //
    buttonMatrix[2] = new ButtonGroup(cont, group, "DTS", optionDTS, NULL);

    // Register a callbacks
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), buttonMatrix[0]->buttons);
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), buttonMatrix[1]->buttons);
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), buttonMatrix[2]->buttons);

    // Draw back button in default location
    DrawBackButton();

    // We have to set the buttons values manually
    // Video Modes
    if(xboxConfig.AudioFlags & XC_AUDIO_FLAGS_MONO)
        lv_btnmatrix_set_btn_ctrl(buttonMatrix[0]->buttons, 0, LV_BTNMATRIX_CTRL_CHECK_STATE);
    else if(xboxConfig.AudioFlags & XC_AUDIO_FLAGS_SURROUND)
        lv_btnmatrix_set_btn_ctrl(buttonMatrix[0]->buttons, 2, LV_BTNMATRIX_CTRL_CHECK_STATE);
    else
        lv_btnmatrix_set_btn_ctrl(buttonMatrix[0]->buttons, 1, LV_BTNMATRIX_CTRL_CHECK_STATE);

    // Dolby Digital (AC3)
    lv_btnmatrix_set_btn_ctrl(buttonMatrix[1]->buttons, (xboxConfig.AudioFlags & XC_AUDIO_FLAGS_ENABLE_AC3) > 0, LV_BTNMATRIX_CTRL_CHECK_STATE);

    // DTS
    lv_btnmatrix_set_btn_ctrl(buttonMatrix[2]->buttons, (xboxConfig.AudioFlags & XC_AUDIO_FLAGS_ENABLE_DTS) > 0, LV_BTNMATRIX_CTRL_CHECK_STATE);

    // Reset warp so that the newly focused object is set as current
    WarpObjectReset(group);
}

SystemAudioSettings::~SystemAudioSettings(void)
{
    // Store values since we can not trust lvgl's events to give us the correct states
    if(lv_btnmatrix_get_btn_ctrl(buttonMatrix[0]->buttons, 1, LV_BTNMATRIX_CTRL_CHECK_STATE)) {
        xboxConfig.SetAudioMode(XC_AUDIO_FLAGS_STEREO);
    } else if(lv_btnmatrix_get_btn_ctrl(buttonMatrix[0]->buttons, 2, LV_BTNMATRIX_CTRL_CHECK_STATE)) {
        xboxConfig.SetAudioMode(XC_AUDIO_FLAGS_SURROUND);
    } else
        xboxConfig.SetAudioMode(XC_AUDIO_FLAGS_MONO);

    xboxConfig.SetAudioAC3(lv_btnmatrix_get_btn_ctrl(buttonMatrix[1]->buttons, 1, LV_BTNMATRIX_CTRL_CHECK_STATE));
    xboxConfig.SetAudioDTS(lv_btnmatrix_get_btn_ctrl(buttonMatrix[2]->buttons, 1, LV_BTNMATRIX_CTRL_CHECK_STATE));

    // Save EEPROM
    xboxConfig.Save();

    // TODO: Clean up all of the other objects.
    lv_obj_del(screen);
}

void SystemAudioSettings::CreateLineBreak()
{
    // Create an array for the points of the line
    static lv_point_t line_points[] = { { 0, 0 }, { 300 - 10, 0 } };

    // Create style
    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, LV_STATE_DEFAULT, 2);
    lv_style_set_line_color(&style_line, LV_STATE_DEFAULT, LV_COLOR_GRAY);
    lv_style_set_line_rounded(&style_line, LV_STATE_DEFAULT, true);

    // Create a line and apply the new style
    lv_obj_t * line1;
    line1 = lv_line_create(cont, NULL);
    lv_line_set_points(line1, line_points, 2);
    lv_obj_add_style(line1, LV_LINE_PART_MAIN, &style_line);
    lv_obj_align(line1, NULL, LV_ALIGN_CENTER, 0, 0);
}

lv_obj_t *SystemAudioSettings::CreateSubSceneButton(const char *text) {
    lv_obj_t *btn = lv_btn_create(cont, NULL);

    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, -40);
    lv_btn_set_fit2(btn, LV_FIT_PARENT, LV_FIT_NONE);
    lv_obj_set_height_fit(btn, 70);

    // TODO: Possible memory leak?
    lv_obj_t *label = lv_label_create(btn, NULL);
    lv_label_set_text(label, text);

    //
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), btn);

    return btn;
}

void SystemAudioSettings::OnObjectEvent(lv_obj_t* obj, lv_event_t event)
{
    // Preform warp check on event
    if(WarpObjectOnEvent(obj, event, group)) { return; }

    if(event == LV_EVENT_KEY) {
        uint32_t event_key = *(uint32_t *)lv_event_get_data();

        if(event_key == LV_KEY_ESC) {
            // Return to previous scene
            load_scene = SCENE::SYSTEM_SETTINGS;
            return;
        }
    }

    // HACK: Escape now if the event was triggered after exit (lvgl event handler can trigger after destructor call)
    if(load_scene != SCENE::SYSTEM_AUDIO_SETTINGS)
        return;

    if(event == LV_EVENT_VALUE_CHANGED) {
        // Audio Mode change (mimics MS dashboard)
        if(obj == buttonMatrix[0]->buttons) {
            // Toggle AC3/DTS options based on surround sound or not
            bool isSurroundSound = lv_btnmatrix_get_active_btn(buttonMatrix[0]->buttons) == 2;

            lv_btnmatrix_set_btn_ctrl(buttonMatrix[1]->buttons, isSurroundSound, LV_BTNMATRIX_CTRL_CHECK_STATE);
            lv_btnmatrix_set_btn_ctrl(buttonMatrix[2]->buttons, isSurroundSound, LV_BTNMATRIX_CTRL_CHECK_STATE);
        } else if(lv_btnmatrix_get_active_btn(buttonMatrix[1]->buttons) == 1 || lv_btnmatrix_get_active_btn(buttonMatrix[2]->buttons) == 1)
            lv_btnmatrix_set_btn_ctrl(buttonMatrix[0]->buttons, 2, LV_BTNMATRIX_CTRL_CHECK_STATE);
    }
}
