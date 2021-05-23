#include <stdio.h>
#include <string.h>
#include <lvgl.h>

#include "ButtonGroup.h"

static lv_res_t MatrixEventHandler(lv_obj_t * obj, lv_signal_t sign, void * param) {
    ButtonGroup* local = static_cast<ButtonGroup *>(obj->user_data);
    return local->OnMatrixEvent(obj, sign, param);
}

static void ButtonEventHandler(lv_obj_t * obj, lv_event_t event) {
    ButtonGroup* scene = static_cast<ButtonGroup *>(obj->user_data);
    scene->OnObjectEvent(obj, event);
}

ButtonGroup::ButtonGroup(lv_obj_t *cont, lv_group_t *group_target, const char *label_text, const char * btnm_map[], uint8_t *value) {
    // Store refrence to group for wrapping
    group = group_target;

    // Create label
    label = lv_label_create(cont, NULL);
    lv_label_set_text(label, label_text);

    buttons = lv_btnmatrix_create(cont, NULL);

    //
    lv_btnmatrix_set_map(buttons, btnm_map);

    //
    lv_btnmatrix_set_one_check(buttons, true);
    lv_btnmatrix_set_btn_ctrl_all(buttons, LV_BTNMATRIX_CTRL_CHECKABLE);

    //
    lv_obj_align(buttons, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(buttons, lv_obj_get_width(cont) - 28, 40);

    // Add matrix to group
    lv_group_add_obj(group, buttons);

    lv_btnmatrix_set_btn_ctrl(buttons, 0, LV_BTNMATRIX_CTRL_CHECK_STATE);

    //
    if(value != NULL)
        lv_btnmatrix_set_btn_ctrl(buttons, *value, LV_BTNMATRIX_CTRL_CHECK_STATE);

    // Register callbacks
   // lv_obj_set_user_data(buttons, static_cast<lv_obj_user_data_t>(this));
    //lv_obj_set_signal_cb(buttons, MatrixEventHandler);
   // lv_obj_set_event_cb(buttons,  ButtonEventHandler);
}

ButtonGroup::~ButtonGroup(void)
{

}

void ButtonGroup::OnObjectEvent(lv_obj_t* obj, lv_event_t event)
{
    printf("Event %d\r\n", event);
}
