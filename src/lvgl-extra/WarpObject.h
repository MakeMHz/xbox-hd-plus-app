#pragma once
#include <lvgl.h>

// Functions
bool WarpObjectOnEvent(lv_obj_t* obj, lv_event_t event, lv_group_t *g);
void WarpObjectReset(lv_group_t *group);

static uint16_t WarpObjectFindButtonFocus(lv_obj_t *object);

// Helper functions
void lv_group_add_obj_warp(lv_group_t *group, lv_event_cb_t event, lv_obj_user_data_t data, lv_obj_t *obj);
