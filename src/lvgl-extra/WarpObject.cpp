#include <stdio.h>
#include <string.h>
#include <lvgl.h>

#include "WarpObject.h"

static lv_obj_t *last_object = NULL;
static uint16_t  last_button = NULL;

void WarpObjectReset(lv_group_t *group) {
    last_object = lv_group_get_focused(group);
    last_button = WarpObjectFindButtonFocus(last_object);
}

bool WarpObjectOnEvent(lv_obj_t* obj, lv_event_t event, lv_group_t *g)
{
    bool update_group_focus = false;
    uint32_t event_key = *(uint32_t *)lv_event_get_data();

    if(event == LV_EVENT_FOCUSED) {
        WarpObjectReset(g);
        printf("LV_EVENT_FOCUSED last_button: %d\r\n", last_button);
    }

    if(event == LV_EVENT_KEY)
    {
        if(
            lv_debug_check_obj_type(obj, "lv_btn") ||
            lv_debug_check_obj_type(obj, "lv_switch")
        ) {
            update_group_focus = true;
        }

        if(lv_debug_check_obj_type(obj, "lv_btnmatrix")) {
            if(last_button == lv_btnmatrix_get_active_btn(obj)) {
                update_group_focus = true;
            }
            last_button = lv_btnmatrix_get_active_btn(obj);
        }
    }

    if(update_group_focus) {
        if(event_key == LV_KEY_UP || event_key == LV_KEY_LEFT)
        {
            lv_group_set_editing(g, false);
            lv_group_focus_prev(g);

            // If the newly focused object is a 'multi-object', then set the focus to the last item.
            lv_obj_t * focused_ohject = lv_group_get_focused(g);
            if(lv_debug_check_obj_type(focused_ohject, "lv_btnmatrix")) {
                lv_btnmatrix_ext_t *ext = (lv_btnmatrix_ext_t *)lv_obj_get_ext_attr(focused_ohject);

                if(ext->btn_cnt > 1) {
                    lv_btnmatrix_set_focused_btn(focused_ohject, ext->btn_cnt - 1);
                    last_button = WarpObjectFindButtonFocus(focused_ohject);
                }
            }
        }

        if(event_key == LV_KEY_DOWN || event_key == LV_KEY_RIGHT)
        {
            lv_group_set_editing(g, false);
            lv_group_focus_next(g);
        }
    }

    // Store refrence to the last object
    last_object = obj;

    return false;
}

static uint16_t WarpObjectFindButtonFocus(lv_obj_t *object) {
    uint16_t focus = 0;

    if(object != NULL && lv_debug_check_obj_type(object, "lv_btnmatrix")) {
        focus = lv_btnmatrix_get_focused_btn(object);
    }

    return focus;
}

void lv_group_add_obj_warp(lv_group_t *group, lv_event_cb_t event, lv_obj_user_data_t data, lv_obj_t *obj)
{
    lv_obj_set_user_data(obj, data);
    lv_obj_set_event_cb(obj, event);

    lv_group_add_obj(group, obj);
}
