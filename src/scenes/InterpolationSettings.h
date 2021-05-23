#pragma once
#include <lvgl.h>
#include "lvgl-extra/ButtonGroup.h"

#include "Scene.h"

class InterpolationSettings : public Scene {
    public:
        InterpolationSettings();
        virtual ~InterpolationSettings();

        void OnObjectEvent(lv_obj_t* obj, lv_event_t event);
    private:
        ButtonGroup *buttonMatrix[4];
};
