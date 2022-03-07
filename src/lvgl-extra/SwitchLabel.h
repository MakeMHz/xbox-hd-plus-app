#pragma once
#include <lvgl.h>

class SwitchLabel {
    public:
        SwitchLabel(lv_obj_t *cont, lv_group_t *group, const char *label_text, uint8_t value);
        ~SwitchLabel();

        lv_obj_t *container;
        lv_obj_t *lv_switch;
        lv_obj_t *label;
    protected:
};
