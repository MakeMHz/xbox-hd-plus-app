#include <stdio.h>
#include <string.h>
#include <lvgl.h>

#include "lvgl-extra/ButtonGroup.h"
#include "lvgl-extra/WarpObject.h"

#include "App.h"
#include "scenes/VideoSettings.h"

static const char * optionResolution[] = { "Original", "480p", "\n", "720p", "1080i", "" };
static const char * optionWidescreen[] = { "Auto", "Force", "Never", "" };
static const char * optionAudio[]      = { "Auto", "Compatibility", "" };;

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

    //
    buttonMatrix[0] = new ButtonGroup(cont, group, "Output Resolution", optionResolution, (uint8_t *)&gEEPROM->current.output_resolution);
    setButtonMtxStyles(buttonMatrix[0]->buttons);
    lv_obj_set_size(buttonMatrix[0]->buttons, lv_obj_get_width(cont) - 28, 100);

    //
    buttonMatrix[1] = new ButtonGroup(cont, group, "4:3 Widescreen", optionWidescreen, (uint8_t *)&gEEPROM->current.widescreen);
    setButtonMtxStyles(buttonMatrix[1]->buttons);

    //
    buttonMatrix[2] = new ButtonGroup(cont, group, "Audio", optionAudio, (uint8_t *)&gEEPROM->current.audo_comp);
    setButtonMtxStyles(buttonMatrix[2]->buttons);

    // Register a callbacks
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), buttonMatrix[0]->buttons);
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), buttonMatrix[1]->buttons);
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), buttonMatrix[2]->buttons);

    // Draw back button in default location
    DrawBackButton();

    // Reset warp so that the newly focused object is set as current
    WarpObjectReset(group);
}

VideoSettings::~VideoSettings(void)
{
    // TODO: Clean up all of the other objects.
    lv_obj_del(screen);
}

void VideoSettings::OnObjectEvent(lv_obj_t* obj, lv_event_t event)
{
    // Preform warp check on event
    if(WarpObjectOnEvent(obj, event, group)) { return; }

    if(event == LV_EVENT_VALUE_CHANGED) {
        if(obj == buttonMatrix[0]->buttons)
            gEEPROM->current.output_resolution = (OUTPUT_RESOLUTION)lv_btnmatrix_get_active_btn(obj);
        if(obj == buttonMatrix[1]->buttons)
            gEEPROM->current.widescreen = (OUTPUT_WIDESCREEN)lv_btnmatrix_get_active_btn(obj);
        if(obj == buttonMatrix[2]->buttons)
            gEEPROM->current.audo_comp = (AUDIO_COMP)lv_btnmatrix_get_active_btn(obj);
    }

    if(event == LV_EVENT_KEY)
    {
        uint32_t event_key = *(uint32_t *)lv_event_get_data();

        if(event_key == LV_KEY_ESC) {
            // Save EEPROM
            gEEPROM->save();

            // Return to previous scene
            load_scene = SCENE::ROOT;
        }
    }
}
