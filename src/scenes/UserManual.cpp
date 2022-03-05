#include <stdio.h>
#include <string.h>
#include <lvgl.h>

#include "lvgl-extra/ButtonGroup.h"
#include "lvgl-extra/WarpObject.h"

#include "App.h"
#include "Assets.h"
#include "scenes/UserManual.h"

static const char textUserManual[] = ""
    "Scan the QR code below or visit\n"
    "https://makemhz.com/hd-guide";

static void ButtonEventHandler(lv_obj_t * obj, lv_event_t event)
{
    UserManual* scene = static_cast<UserManual *>(obj->user_data);
    scene->OnObjectEvent(obj, event);
}

UserManual::UserManual()
{
    // Set the size and position of our scene
    lv_obj_set_size(cont, 320, 420);
    lv_obj_set_pos(cont, 380, 20);
    lv_cont_set_layout(cont, LV_LAYOUT_COLUMN_LEFT);

    labelHeader = lv_label_create(cont, NULL);
    lv_label_set_text(labelHeader, "User Manual");

    labelText = lv_label_create(cont, NULL);
    lv_label_set_text(labelText, textUserManual);

    // Draw QR code
    imageQR = lv_img_create(cont, NULL);
    lv_img_set_src(imageQR, &UserManualQR);

    // Register a callbacks
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), labelHeader);

    // Draw back button in default location
    DrawBackButton();

    // Reset warp so that the newly focused object is set as current
    WarpObjectReset(group);
}

UserManual::~UserManual(void) {
    // TODO: Clean up all of the other objects.
    lv_obj_del(screen);
}

void UserManual::OnObjectEvent(lv_obj_t* obj, lv_event_t event)
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
