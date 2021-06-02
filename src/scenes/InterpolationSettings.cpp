#include <stdio.h>
#include <string.h>
#include <lvgl.h>

#include "lvgl-extra/ButtonGroup.h"
#include "lvgl-extra/WarpObject.h"

#include "App.h"
#include "scenes/InterpolationSettings.h"

static const char *optionScaler[] = { "Truncate", "Bilinear", "" };
static const char *optionWeight[] = { "Truncate", "Round", "" };

static void ButtonEventHandler(lv_obj_t * obj, lv_event_t event) {
    InterpolationSettings* scene = static_cast<InterpolationSettings *>(obj->user_data);
    scene->OnObjectEvent(obj, event);
}

InterpolationSettings::InterpolationSettings()
{
    // Set the size and position of our scene
    lv_obj_set_size(cont, 320, 420);
    lv_obj_set_pos(screen, 380, 20);
    lv_cont_set_layout(cont, LV_LAYOUT_COLUMN_LEFT);

    //
    buttonMatrix[0] = new ButtonGroup(cont, group, "X Scale", optionScaler, (uint8_t *)&gEEPROM->current.interpolation_x_scale);
    setButtonMtxStyles(buttonMatrix[0]->buttons);

    //
    buttonMatrix[1] = new ButtonGroup(cont, group, "Y Scale", optionScaler, (uint8_t *)&gEEPROM->current.interpolation_y_scale);
    setButtonMtxStyles(buttonMatrix[1]->buttons);

    //
    buttonMatrix[2] = new ButtonGroup(cont, group, "X Weight", optionWeight,(uint8_t *)&gEEPROM->current.interpolation_x_weight);
    setButtonMtxStyles(buttonMatrix[2]->buttons);

    //
    buttonMatrix[3] = new ButtonGroup(cont, group, "Y Weight", optionWeight, (uint8_t *)&gEEPROM->current.interpolation_y_weight);
    setButtonMtxStyles(buttonMatrix[3]->buttons);

    // Register a callbacks
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), buttonMatrix[0]->buttons);
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), buttonMatrix[1]->buttons);
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), buttonMatrix[2]->buttons);
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), buttonMatrix[3]->buttons);

    // Draw back button in default location
    DrawBackButton();

    // Reset warp so that the newly focused object is set as current
    WarpObjectReset(group);
}

InterpolationSettings::~InterpolationSettings(void)
{
    // TODO: Clean up all of the other objects.
    lv_obj_del(screen);
}

void InterpolationSettings::OnObjectEvent(lv_obj_t* obj, lv_event_t event)
{
    // Preform warp check on event
    if(WarpObjectOnEvent(obj, event, group)) { return; }

    if(event == LV_EVENT_VALUE_CHANGED) {
        if(obj == buttonMatrix[0]->buttons)
            gEEPROM->current.interpolation_x_scale = (INTERPOLATION_X_SCALE)lv_btnmatrix_get_active_btn(obj);
        if(obj == buttonMatrix[1]->buttons)
            gEEPROM->current.interpolation_y_scale = (INTERPOLATION_Y_SCALE)lv_btnmatrix_get_active_btn(obj);
        if(obj == buttonMatrix[2]->buttons)
            gEEPROM->current.interpolation_x_weight = (INTERPOLATION_X_WEIGHT)lv_btnmatrix_get_active_btn(obj);
        if(obj == buttonMatrix[3]->buttons)
            gEEPROM->current.interpolation_y_weight = (INTERPOLATION_Y_WEIGHT)lv_btnmatrix_get_active_btn(obj);
    }

    if(event == LV_EVENT_KEY)
    {
        uint32_t event_key = *(uint32_t *)lv_event_get_data();

        if(event_key == LV_KEY_ESC) {
            // Save EEPROM
            gEEPROM->save();

            // Return to previous scene
            load_scene = SCENE::ADVANCED_SETTINGS;
        }
    }
}
