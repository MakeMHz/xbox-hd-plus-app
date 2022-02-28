#include <stdio.h>
#include <string.h>
#include <lv_ex_conf.h>
#include <lvgl.h>

#include "Assets.h"
#include "Scene.h"

Scene::Scene()
{
    parent_screen = lv_scr_act();

    screen = lv_obj_create(parent_screen, NULL);

    lv_obj_set_style_local_bg_opa(screen, LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);
    lv_obj_set_style_local_border_width(screen, LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_outline_width(screen, 0, LV_STATE_DEFAULT, 0);

    lv_obj_set_size(screen, lv_obj_get_width(parent_screen), lv_obj_get_height(parent_screen));
    lv_obj_set_pos(screen, 0, 0);

    // Register input device
    lv_indev_t *sdl_indev = lv_indev_get_next(NULL);
    group = lv_group_create();
    if(sdl_indev != NULL)
        lv_indev_set_group(sdl_indev, group);

    //
    lv_group_set_editing(group, false);

    //
    cont = lv_cont_create(screen, NULL);

    lv_obj_set_style_local_bg_opa(cont, LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_40);
    lv_obj_set_style_local_border_width(cont, LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, 0);
}

void Scene::DrawBackButton()
{
    DrawBackButton("Back");
}

void Scene::DrawBackButton(const char *text)
{
    back_button_image = lv_img_create(screen, NULL);
    lv_img_set_src(back_button_image, &Xbox_B2_32);
    lv_obj_align(back_button_image, cont, LV_ALIGN_IN_BOTTOM_RIGHT, -60, -16);

    back_button_label = lv_label_create(screen, NULL);
    lv_label_set_text(back_button_label, text);
    lv_obj_align(back_button_label, back_button_image, LV_ALIGN_CENTER, 42, 0);
    lv_obj_set_width(back_button_label, 200);
}
