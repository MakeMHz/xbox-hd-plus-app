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

VideoSettings::VideoSettings() {
    // Set the size and position of our scene
    lv_obj_set_size(cont, 320, 420);
    lv_obj_set_pos(cont, 380, 20);

    lv_cont_set_layout(cont, LV_LAYOUT_COLUMN_LEFT);

    //
    uint8_t value = 1;
    buttonMatrix[0] = new ButtonGroup(cont, group, "Output Resolution", optionResolution, &value);
    setButtonMtxStyles(buttonMatrix[0]->buttons);
    lv_obj_set_size(buttonMatrix[0]->buttons, lv_obj_get_width(cont) - 28, 100);

    //
    buttonMatrix[1] = new ButtonGroup(cont, group, "4:3 Widescreen", optionWidescreen, &value);
    setButtonMtxStyles(buttonMatrix[1]->buttons);

    //
    buttonMatrix[2] = new ButtonGroup(cont, group, "Audio", optionAudio, &value);
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

    if(event == LV_EVENT_KEY)
    {
        uint32_t event_key = *(uint32_t *)lv_event_get_data();

        if(event_key == LV_KEY_ESC)
            load_scene = SCENE::ROOT;
    }
}
