#include <stdio.h>
#include <string.h>
#include <lvgl.h>

#include "lvgl-extra/ButtonGroup.h"
#include "lvgl-extra/WarpObject.h"

#include "App.h"
#include "scenes/AdvanceSettings.h"

static const char *optionColorSpace[] = { "Auto", "YCBCR", "RGB", "" };
static const char *optionUpscaling[]  = { "Truncate", "Bilinear", "" };

static void ButtonEventHandler(lv_obj_t * obj, lv_event_t event) {
    AdvanceSettings* scene = static_cast<AdvanceSettings *>(obj->user_data);
    scene->OnObjectEvent(obj, event);
}

AdvanceSettings::AdvanceSettings()
{
    // Set the size and position of our scene
    lv_obj_set_size(cont, 320, 420);
    lv_obj_set_pos(screen, 380, 20);
    lv_cont_set_layout(cont, LV_LAYOUT_COLUMN_LEFT);

    //
    buttonMatrix[0] = new ButtonGroup(cont, group, "Colorspace", optionColorSpace, (uint8_t *)&gEEPROM->current.colorspace);
    setButtonMtxStyles(buttonMatrix[0]->buttons);

    //
    buttonMatrix[1] = new ButtonGroup(cont, group, "Upscaling Interpolation", optionUpscaling, (uint8_t *)&gEEPROM->current.interpolation);
    setButtonMtxStyles(buttonMatrix[1]->buttons);

    // Add line break between button options and button scenes
    CreateLineBreak();

    // Draw our child scene buttons
    btnAdvanceInterpolation = CreateSubSceneButton("Advance Interpolation");
    btnColospace            = CreateSubSceneButton("Color Correction");
    btnOverscan             = CreateSubSceneButton("Overscan Correction");

    // Register a callbacks
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), buttonMatrix[0]->buttons);
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), buttonMatrix[1]->buttons);

    // Draw back button in default location
    DrawBackButton();

    // Reset warp so that the newly focused object is set as current
    WarpObjectReset(group);
}

void AdvanceSettings::CreateLineBreak()
{
    // Create an array for the points of the line
    static lv_point_t line_points[] = { { 0, 0 }, { 300 - 10, 0 } };

    // Create style
    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, LV_STATE_DEFAULT, 2);
    lv_style_set_line_color(&style_line, LV_STATE_DEFAULT, LV_COLOR_BLACK);
    lv_style_set_line_rounded(&style_line, LV_STATE_DEFAULT, true);

    // Create a line and apply the new style
    lv_obj_t * line1;
    line1 = lv_line_create(cont, NULL);
    lv_line_set_points(line1, line_points, 2);
    lv_obj_add_style(line1, LV_LINE_PART_MAIN, &style_line);
    lv_obj_align(line1, NULL, LV_ALIGN_CENTER, 0, 0);
}

lv_obj_t *AdvanceSettings::CreateSubSceneButton(const char *text) {
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

AdvanceSettings::~AdvanceSettings(void)
{
    // TODO: Clean up all of the other objects.
    lv_obj_del(screen);
}

void AdvanceSettings::OnObjectEvent(lv_obj_t* obj, lv_event_t event)
{
    // Preform warp check on event
    if(WarpObjectOnEvent(obj, event, group)) { return; }

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

    if(event == LV_EVENT_VALUE_CHANGED) {
        if(obj == buttonMatrix[0]->buttons)
            gEEPROM->current.colorspace = (COLORSPACE)lv_btnmatrix_get_active_btn(obj);
        if(obj == buttonMatrix[1]->buttons)
            gEEPROM->current.interpolation = (INTERPOLATION)lv_btnmatrix_get_active_btn(obj);
    }

    if(event == LV_EVENT_PRESSED)
    {
        if(obj == btnAdvanceInterpolation)
            load_scene = SCENE::INTERPOLATION_SETTINGS;
        if(obj == btnColospace)
            load_scene = SCENE::COLOR_CORRECTION;
        if(obj == btnOverscan)
            load_scene = SCENE::OVERSCAN_CORRECTION;
    }
}
