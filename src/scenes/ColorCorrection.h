#pragma once
#include <lvgl.h>
#include "lvgl-extra/ButtonGroup.h"

#include "Scene.h"

class ColorCorrection : public Scene {
    public:
        ColorCorrection();
        virtual ~ColorCorrection();

        void OnObjectEvent(lv_obj_t* obj, lv_event_t event);
    private:
        lv_obj_t *label1;
};
