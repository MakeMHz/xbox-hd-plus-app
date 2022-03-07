#include <stdio.h>
#include <string.h>
#include <lvgl.h>

#include "lvgl-extra/ButtonGroup.h"
#include "lvgl-extra/WarpObject.h"

#include "App.h"
#include "scenes/InterpolationSettings.h"

static const char *optionScaler[] = { "Bilinear", "Truncate", "" };
static const char *optionWeight[] = { "Round", "Truncate", "" };

static void ButtonEventHandler(lv_obj_t * obj, lv_event_t event) {
    InterpolationSettings* scene = static_cast<InterpolationSettings *>(obj->user_data);
    scene->OnObjectEvent(obj, event);
}

InterpolationSettings::InterpolationSettings()
{
    // Set the size and position of our scene
    lv_obj_set_size(cont, 320, 420);
    lv_obj_set_pos(cont, 380, 20);
    lv_cont_set_layout(cont, LV_LAYOUT_COLUMN_LEFT);

    //
    buttonMatrix[0] = new ButtonGroup(cont, group, "X Scale", optionScaler, (uint8_t *)&gEEPROM->current.interpolation_x_scale);

    //
    buttonMatrix[1] = new ButtonGroup(cont, group, "Y Scale", optionScaler, (uint8_t *)&gEEPROM->current.interpolation_y_scale);

    //
    buttonMatrix[2] = new ButtonGroup(cont, group, "X Weight", optionWeight,(uint8_t *)&gEEPROM->current.interpolation_x_weight);

    //
    buttonMatrix[3] = new ButtonGroup(cont, group, "Y Weight", optionWeight, (uint8_t *)&gEEPROM->current.interpolation_y_weight);

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
    // Store values since we can not trust lvgl's events to give us the correct states
    gEEPROM->current.interpolation_x_scale  = (uint8_t)lv_btnmatrix_get_btn_ctrl(buttonMatrix[0]->buttons, 1, LV_BTNMATRIX_CTRL_CHECK_STATE);
    gEEPROM->current.interpolation_y_scale  = (uint8_t)lv_btnmatrix_get_btn_ctrl(buttonMatrix[1]->buttons, 1, LV_BTNMATRIX_CTRL_CHECK_STATE);
    gEEPROM->current.interpolation_x_weight = (uint8_t)lv_btnmatrix_get_btn_ctrl(buttonMatrix[2]->buttons, 1, LV_BTNMATRIX_CTRL_CHECK_STATE);
    gEEPROM->current.interpolation_y_weight = (uint8_t)lv_btnmatrix_get_btn_ctrl(buttonMatrix[3]->buttons, 1, LV_BTNMATRIX_CTRL_CHECK_STATE);

    // Save EEPROM
    gEEPROM->save();

    // TODO: Clean up all of the other objects.
    lv_obj_del(screen);
}

void InterpolationSettings::OnObjectEvent(lv_obj_t* obj, lv_event_t event)
{
    // Preform warp check on event
    if(WarpObjectOnEvent(obj, event, group)) { return; }

    // HACK: Escape now if the event was triggered after exit (lvgl event handler can trigger after destructor call)
    if(load_scene != SCENE::INTERPOLATION_SETTINGS)
        return;

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
