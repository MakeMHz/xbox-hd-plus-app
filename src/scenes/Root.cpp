#include <stdio.h>
#include <string.h>
#include <hal/xbox.h>
#include <lvgl.h>
#include <lv_sdl_drv_input.h>

#include "lvgl-extra/ButtonGroup.h"
#include "lvgl-extra/WarpObject.h"

#include "App.h"
#include "Update.h"
#include "scenes/Root.h"

extern Update *update;

static void ButtonEventHandler(lv_obj_t * obj, lv_event_t event)
{
    SceneRoot* scene = static_cast<SceneRoot *>(obj->user_data);
    scene->OnObjectEvent(obj, event);
}

SceneRoot::SceneRoot()
{
    // Set the size and position of our scene
    lv_obj_set_size(cont, 320, 420);
    lv_obj_set_pos(cont, 380, 20);
    lv_cont_set_layout(cont, LV_LAYOUT_COLUMN_LEFT);

    // Draw our child scene buttons
    //btnSelfTest        = CreateSubSceneButton("Self Test");
    btnSystemSettings  = CreateSubSceneButton("System Settings");
    btnVideoSettings   = CreateSubSceneButton("Video Settings");
    btnAdvanceSettings = CreateSubSceneButton("Advance Settings");
    btnFirmware        = CreateSubSceneButton("Check for Update");
    btnUserManual      = CreateSubSceneButton("User Manual");
    btnAbout           = CreateSubSceneButton("About & Credits");

    // Draw back button in default location
    DrawBackButton("Exit");

    // Reset warp so that the newly focused object is set as current
    WarpObjectReset(group);
}

lv_obj_t *SceneRoot::CreateSubSceneButton(const char *text) {
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

SceneRoot::~SceneRoot(void)
{
    // TODO: Clean up all of the other objects.
    lv_obj_del(screen);
}

void SceneRoot::OnObjectEvent(lv_obj_t* obj, lv_event_t event)
{
    // Check if the update process is running
    if(update->current_state != update->UPDATE_LOAD_INIT) {
        // If the update is 'done', or has failed, then allow the user to reboot with the B button.
        if(update->current_state == update->UPDATE_DONE && event == LV_EVENT_CANCEL) {
            XLaunchXBE("C:\\xboxhd\\default.xbe");
        }
        return;
    }

    // Preform warp check on event
    if(WarpObjectOnEvent(obj, event, group)) { return; }

    if(event == LV_EVENT_PRESSED)
    {
        if(obj == btnSelfTest) {
            load_scene = SCENE::SELF_TEST;
        }
        if(obj == btnSystemSettings) {
            load_scene = SCENE::SYSTEM_SETTINGS;
        }
        if(obj == btnVideoSettings) {
            load_scene = SCENE::VIDEO_SETTINGS;
        }
        if(obj == btnAdvanceSettings) {
            load_scene = SCENE::ADVANCED_SETTINGS;
        }
        if(obj == btnUserManual) {
            load_scene = SCENE::USER_MANUAL;
        }
        if(obj == btnFirmware) {
            update->check();
        }
        if(obj == btnAbout) {
            load_scene = SCENE::CREDITS;
        }
    }

    // Exit app on B press
    if(event == LV_EVENT_CANCEL) {
        set_quit_event(SHUTDOWN);
    }
}
