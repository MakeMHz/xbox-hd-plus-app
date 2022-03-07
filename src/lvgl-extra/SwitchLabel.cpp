#include <stdio.h>
#include <string.h>
#include <lvgl.h>

#include "SwitchLabel.h"

SwitchLabel::SwitchLabel(lv_obj_t *cont, lv_group_t *group, const char *label_text, uint8_t value) {
    container = lv_cont_create(cont, NULL);

    // TODO: Implement using styles
    lv_obj_set_style_local_border_width(container, LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_margin_all(container, LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, 0);
    lv_obj_set_style_local_pad_hor(container, LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, 2);
    lv_obj_set_style_local_pad_ver(container, LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, 2);
    lv_obj_set_style_local_bg_opa(container, LV_OBJMASK_PART_MAIN, LV_STATE_DEFAULT, LV_OPA_TRANSP);

    lv_cont_set_fit2(container, LV_FIT_MAX, LV_FIT_TIGHT);
    lv_cont_set_layout(container, LV_LAYOUT_ROW_MID);

    lv_switch = lv_switch_create(container, NULL);

    label = lv_label_create(container, NULL);
    lv_label_set_text(label, label_text);

    // Add container to group
    lv_group_add_obj(group, lv_switch);

    //
    if(value != NULL)
        lv_switch_on(lv_switch, LV_ANIM_OFF);
    else
        lv_switch_off(lv_switch, LV_ANIM_OFF);
}

SwitchLabel::~SwitchLabel(void) {
    lv_obj_del(container);
}
