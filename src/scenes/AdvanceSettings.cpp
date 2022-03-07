#include <stdio.h>
#include <string.h>
#include <lvgl.h>

#include "lvgl-extra/ButtonGroup.h"
#include "lvgl-extra/WarpObject.h"

#include "App.h"
#include "scenes/AdvanceSettings.h"

static const char *optionColorSpace[] = { "YCBCR", "RGB", "" };
static const char *optionUpscaling[]  = { "Bilinear", "Truncate", "" };
static const char *optionAutoBlank[]  = { "Disabled", "Enabled", "" };

static const char helpAutoBlank[] =
    "This option controls if video output should automatically be blanked, blacked out, "
    "when a software title calls AV_OPTION_BLANK_SCREEN and during kernel calls to AvSetDisplayMode().\n\n"
    "This option should be safe to leave on and is left for debugging/exploration purposes.";

static const char helpAutoRegionSwitch[] =
    "With 'Auto Region Switching' enabled, the HD+ kernel patch will detect when a software title "
    "is region locked and will automatically patch the system region in memory.\n\n"
    "Some non-NTSC software may ignore the region setting and run at a higher resolution under NTSC. "
    "For those games it should be safe to turn off this option.\n\n"
    "For example, Dead or Alive 3 (EUR) ignores the console's system region and will run "
    "natively at 480p/NTSC if the system is set to NTSC.\n\n"
    "This option should be safe to leave on and is left for debugging/exploration purposes.";

static void ButtonEventHandler(lv_obj_t * obj, lv_event_t event) {
    AdvanceSettings* scene = static_cast<AdvanceSettings *>(obj->user_data);
    scene->OnObjectEvent(obj, event);
}

AdvanceSettings::AdvanceSettings()
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
    buttonMatrix[0] = new ButtonGroup(cont, group, "HDMI Colorspace", optionColorSpace, (uint8_t *)&gEEPROM->current.colorspace);
    buttonMatrix[1] = new ButtonGroup(cont, group, "Upscaling Interpolation", optionUpscaling, (uint8_t *)&gEEPROM->current.interpolation);

    btnAutoVideoBlanking = new SwitchLabel(cont, group, "Auto Video Blanking", gEEPROM->current.auto_video_blank);
    btnAutoRegionSwitch  = new SwitchLabel(cont, group, "Auto Region Switching", gEEPROM->current.auto_region_switch);

    // Add line break between button options and button scenes
    CreateLineBreak();

    // Register a callbacks
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), buttonMatrix[0]->buttons);
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), buttonMatrix[1]->buttons);

    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), btnAutoVideoBlanking->lv_switch);
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), btnAutoRegionSwitch->lv_switch);

    // Draw our child scene buttons
    btnAdvanceInterpolation  = CreateSubSceneButton("Advance Interpolation");

    // Draw back button in default location
    DrawBackButton();

    // Update helper text
    UpdateHelperText();

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
    lv_style_set_line_color(&style_line, LV_STATE_DEFAULT, LV_COLOR_GRAY);
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

    //
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), btn);

    return btn;
}

AdvanceSettings::~AdvanceSettings(void)
{
    // Store values since we can not trust lvgl's events to give us the correct states
    gEEPROM->current.colorspace         = (uint8_t)lv_btnmatrix_get_btn_ctrl(buttonMatrix[0]->buttons, 1, LV_BTNMATRIX_CTRL_CHECK_STATE);
    gEEPROM->current.interpolation      = (uint8_t)lv_btnmatrix_get_btn_ctrl(buttonMatrix[1]->buttons, 1, LV_BTNMATRIX_CTRL_CHECK_STATE);
    gEEPROM->current.auto_video_blank   = (uint8_t)lv_switch_get_state(btnAutoVideoBlanking->lv_switch);
    gEEPROM->current.auto_region_switch = (uint8_t)lv_switch_get_state(btnAutoRegionSwitch->lv_switch);

    // Save EEPROM
    gEEPROM->save();

    // TODO: Clean up all of the other objects.
    lv_obj_del(screen);
}

void AdvanceSettings::UpdateHelperText() {
    lv_obj_t *focus = lv_group_get_focused(group);

    if(focus == btnAutoVideoBlanking->lv_switch) {
        lv_label_set_text(contHelpLabel, helpAutoBlank);
        lv_obj_set_hidden(contHelp, false);
    } else if(focus == btnAutoRegionSwitch->lv_switch) {
        lv_label_set_text(contHelpLabel, helpAutoRegionSwitch);
        lv_obj_set_hidden(contHelp, false);
    } else
        lv_obj_set_hidden(contHelp, true);
};

void AdvanceSettings::OnObjectEvent(lv_obj_t* obj, lv_event_t event)
{
    // Preform warp check on event
    if(WarpObjectOnEvent(obj, event, group)) { return; }

    if(event == LV_EVENT_KEY)
    {
        uint32_t event_key = *(uint32_t *)lv_event_get_data();

        if(event_key == LV_KEY_ESC) {
            // Return to previous scene
            load_scene = SCENE::ROOT;
        }
    }

    // HACK: Escape now if the event was triggered after exit (lvgl event handler can trigger after destructor call)
    if(load_scene != SCENE::ADVANCED_SETTINGS)
        return;

    if(event == LV_EVENT_FOCUSED)
        UpdateHelperText();

    if(event == LV_EVENT_PRESSED) {
        if(obj == btnAdvanceInterpolation)
            load_scene = SCENE::INTERPOLATION_SETTINGS;
    }
}
