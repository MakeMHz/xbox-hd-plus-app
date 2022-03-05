#include <stdio.h>
#include <string.h>
#include <lvgl.h>
#include <xboxkrnl/xboxkrnl.h>

#include "lvgl-extra/ButtonGroup.h"
#include "lvgl-extra/WarpObject.h"

#include "App.h"
#include "Generated/Contributors.h"
#include "scenes/Credits.h"

static const char textCredits[] = ""
    "The XboxHD+ app is developed and distrubuted by MakeMHz LLC.\n\n"
    "The XboxHD+ app is developed using the nxdk toolchain and wouldn't be "
    "possible without their amazing work. <3\n\n"
    "-------------------------------\n\n"
    "xbox-hd-app\n"
    "Copyright (C) 2022  MakeMHz LLC\n"
    "\n"
    "This program is free software: you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
    "the Free Software Foundation, either version 3 of the License, or\n"
    "(at your option) any later version.\n"
    "\n"
    "This program is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "GNU General Public License for more details.\n"
    "\n"
    "You should have received a copy of the GNU General Public License\n"
    "along with this program.  If not, see <https://www.gnu.org/licenses/>.\n";

static void ButtonEventHandler(lv_obj_t * obj, lv_event_t event)
{
    Credits* scene = static_cast<Credits *>(obj->user_data);
    scene->OnObjectEvent(obj, event);
}


// TODO: Move into class
static uint32_t ContributorListCounter = rand() % ContributorsCount;

static void AnimationEventHandler(lv_obj_t *obj, lv_anim_value_t time) {
    // HACK: lv_anim_set_ready_cb() broke during an lvgl, so we're doing the text update here instead.
    if(time < 3) {
        // Generate a new string
        ContributorListCounter++; // Start from a random index
        lv_obj_set_style_local_text_opa(obj, LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, 0);
        lv_label_set_text_fmt(obj, "Thank You!\n%s", Contributors[ContributorListCounter % ContributorsCount]);
    }

    // Fade text in and out
    if(time < 256)
        lv_obj_set_style_local_text_opa(obj, LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, time);
    else if(time < 512)
        lv_obj_set_style_local_text_opa(obj, LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, 255 - (time - 256));
}

static void AnimationReadyEventHandler(lv_anim_t *a) {
    auto obj = (lv_obj_t *)a->var;

    // Generate a new string
    ContributorListCounter++; // Start from a random index
    lv_obj_set_style_local_text_opa(obj, LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_label_set_text_fmt(obj, "Thank You!\n%s", Contributors[ContributorListCounter % ContributorsCount]);
}

Credits::Credits()
{
    // Set the size and position of our scene
    lv_obj_set_size(cont, 320, 420);
    lv_obj_set_pos(cont, 380, 20);
    lv_cont_set_layout(cont, LV_LAYOUT_COLUMN_LEFT);

    labelHeader = lv_label_create(cont, NULL);
    lv_label_set_align(labelHeader, LV_LABEL_ALIGN_CENTER);
    lv_label_set_text(labelHeader, "About & Credits");
    lv_obj_set_width(labelHeader, 150);

    pageMain = lv_page_create(cont, NULL);
    lv_obj_set_size(pageMain, 290, 310);
    lv_obj_align(pageMain, NULL, LV_ALIGN_CENTER, 0, 0);

    //
    labelCredits = lv_label_create(pageMain, NULL);
    lv_label_set_long_mode(labelCredits, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(labelCredits, lv_page_get_width_fit(pageMain));
    lv_label_set_text(labelCredits, textCredits);

    // Set up 'Thank You' label
    labelThankYou = lv_label_create(cont, NULL);
    lv_obj_set_pos(labelThankYou, 30, 300);

    // Set up 'Thank You' animation
    lv_anim_init(&a);
    lv_anim_set_exec_cb(&a, (lv_anim_exec_xcb_t) AnimationEventHandler);

    lv_anim_set_var(&a, labelThankYou);
    lv_anim_set_time(&a, 4000);
    lv_anim_set_values(&a, 0, 512);
    lv_anim_set_repeat_delay(&a, 1000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_start(&a);

    // Register a callbacks
    lv_group_add_obj_warp(group, ButtonEventHandler, static_cast<lv_obj_user_data_t>(this), pageMain);

    // Draw back button in default location
    DrawBackButton();

    // Reset warp so that the newly focused object is set as current
    WarpObjectReset(group);
}

Credits::~Credits(void) {
    // TODO: Clean up all of the other objects.
    lv_obj_del(screen);
}

void Credits::OnObjectEvent(lv_obj_t* obj, lv_event_t event)
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
