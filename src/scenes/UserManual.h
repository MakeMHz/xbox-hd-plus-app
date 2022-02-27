#pragma once
#include <lvgl.h>

#include "Scene.h"

class UserManual : public Scene {
    public:
        UserManual();
        virtual ~UserManual();

        void OnObjectEvent(lv_obj_t* obj, lv_event_t event);
    private:
        lv_obj_t *labelHeader;
        lv_obj_t *labelText;
        lv_obj_t *imageQR;
};
