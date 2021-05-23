#pragma once
#include <lvgl.h>

class ButtonGroup {
    public:
        ButtonGroup(lv_obj_t *cont, lv_group_t *group_target, const char *label_text, const char * btnm_map[], uint8_t *value);
        ~ButtonGroup();

        lv_res_t OnMatrixEvent(lv_obj_t * btnm, lv_signal_t sign, void * param);
        void OnObjectEvent(lv_obj_t* obj, lv_event_t event);

        lv_obj_t *buttons;
    protected:

    private:
        lv_obj_t * label;
        lv_signal_cb_t signal_cb;
        lv_group_t *group;
};
