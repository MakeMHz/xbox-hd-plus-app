#include <stdio.h>
#include <string.h>
#include <lvgl.h>
#include <xboxkrnl/xboxkrnl.h>
#include <windows.h>

#include "App.h"
#include "XboxConfig.h"

#include "lvgl-extra/ButtonGroup.h"
#include "lvgl-extra/WarpObject.h"

#include "scenes/SystemSettings.h"

static const char * optionResolution[] = { "480p", "720p", "1080i", "" };
static const char * optionWidescreen[] = { "Normal", "Letterbox", "\n", "Widescreen", "" };
static const char * optionAudio[]      = { "Compatibility", "Auto", "" };;

static void ButtonEventHandler(lv_obj_t * obj, lv_event_t event) {
    SystemSettings* scene = static_cast<SystemSettings *>(obj->user_data);
    scene->OnObjectEvent(obj, event);
}

SystemSettings::SystemSettings()
{
    // Set the size and position of our scene
    lv_obj_set_size(cont, 320, 420);
    lv_obj_set_pos(cont, 380, 20);

    lv_cont_set_layout(cont, LV_LAYOUT_COLUMN_LEFT);

    //
    buttonMatrix[0] = new ButtonGroup(cont, group, "Video Modes", optionResolution, NULL);
    lv_btnmatrix_set_one_check(buttonMatrix[0]->buttons, false);
    setButtonMtxStyles(buttonMatrix[0]->buttons);

    //
    buttonMatrix[1] = new ButtonGroup(cont, group, "Aspect Ratio", optionWidescreen, NULL);
    setButtonMtxStyles(buttonMatrix[1]->buttons);
    lv_obj_set_size(buttonMatrix[1]->buttons, lv_obj_get_width(cont) - 28, 100);

    // Add line break between button options and button scenes
    CreateLineBreak();

    btnSystemAudioSettings = CreateSubSceneButton("Audio Settings");

    // Register a callbacks
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), buttonMatrix[0]->buttons);
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), buttonMatrix[1]->buttons);

    // Draw back button in default location
    DrawBackButton();

    // Reset warp so that the newly focused object is set as current
    WarpObjectReset(group);

    // We have to set the buttons values manually
    // Video Modes
    lv_btnmatrix_set_btn_ctrl_all(buttonMatrix[0]->buttons, LV_BTNMATRIX_CTRL_CHECK_STATE);

    if(xboxConfig.VideoFlags & XC_VIDEO_FLAGS_HDTV_480p)
        lv_btnmatrix_set_btn_ctrl(buttonMatrix[0]->buttons, 0, LV_BTNMATRIX_CTRL_CHECK_STATE);
    if(xboxConfig.VideoFlags & XC_VIDEO_FLAGS_HDTV_720p)
        lv_btnmatrix_set_btn_ctrl(buttonMatrix[0]->buttons, 1, LV_BTNMATRIX_CTRL_CHECK_STATE);
    if(xboxConfig.VideoFlags & XC_VIDEO_FLAGS_HDTV_1080i)
        lv_btnmatrix_set_btn_ctrl(buttonMatrix[0]->buttons, 2, LV_BTNMATRIX_CTRL_CHECK_STATE);

    // Aspect Ratio
    if(xboxConfig.VideoFlags & XC_VIDEO_FLAGS_LETTERBOX)
        lv_btnmatrix_set_btn_ctrl(buttonMatrix[1]->buttons, 1, LV_BTNMATRIX_CTRL_CHECK_STATE);
    else if(xboxConfig.VideoFlags & XC_VIDEO_FLAGS_WIDESCREEN)
        lv_btnmatrix_set_btn_ctrl(buttonMatrix[1]->buttons, 2, LV_BTNMATRIX_CTRL_CHECK_STATE);
    else
        lv_btnmatrix_set_btn_ctrl(buttonMatrix[1]->buttons, 0, LV_BTNMATRIX_CTRL_CHECK_STATE);
}

SystemSettings::~SystemSettings(void)
{
    // Save EEPROM
    xboxConfig.Save();

    // TODO: Clean up all of the other objects.
    lv_obj_del(screen);
}

void SystemSettings::CreateLineBreak()
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

lv_obj_t *SystemSettings::CreateSubSceneButton(const char *text) {
    lv_obj_t *btn = lv_btn_create(cont, NULL);

    lv_obj_align(btn, NULL, LV_ALIGN_CENTER, 0, -40);
    lv_btn_set_fit2(btn, LV_FIT_PARENT, LV_FIT_NONE);
    lv_obj_set_height_fit(btn, 70);

    // TODO: Possible memory leak?
    lv_obj_t *label = lv_label_create(btn, NULL);
    lv_label_set_text(label, text);

    // TODO: Move to proper styles
    setButtonStyles(btn);

    //
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), btn);

    //lv_obj_set_event_cb(btn1, event_handler);

    return btn;
}

void SystemSettings::OnObjectEvent(lv_obj_t* obj, lv_event_t event)
{
    // Preform warp check on event
    if(WarpObjectOnEvent(obj, event, group)) { return; }

    if(event == LV_EVENT_KEY) {
        uint32_t event_key = *(uint32_t *)lv_event_get_data();

        if(event_key == LV_KEY_ESC) {
            // Return to previous scene
            load_scene = SCENE::ROOT;
        }
    }

    if(event == LV_EVENT_VALUE_CHANGED) {
        xboxConfig.SetVideo480p(lv_btnmatrix_get_btn_ctrl(buttonMatrix[0]->buttons, 0, LV_BTNMATRIX_CTRL_CHECK_STATE));
        xboxConfig.SetVideo720p(lv_btnmatrix_get_btn_ctrl(buttonMatrix[0]->buttons, 1, LV_BTNMATRIX_CTRL_CHECK_STATE));
        xboxConfig.SetVideo1080i(lv_btnmatrix_get_btn_ctrl(buttonMatrix[0]->buttons, 2, LV_BTNMATRIX_CTRL_CHECK_STATE));

        if(lv_btnmatrix_get_active_btn(buttonMatrix[1]->buttons) == 1) {
            xboxConfig.SetVideoAspectRatio(XC_VIDEO_FLAGS_LETTERBOX);
        } else if(lv_btnmatrix_get_active_btn(buttonMatrix[1]->buttons) == 2) {
            xboxConfig.SetVideoAspectRatio(XC_VIDEO_FLAGS_WIDESCREEN);
        } else
            xboxConfig.SetVideoAspectRatio(0);
    }
}