#pragma once
#include <lvgl.h>
#include "lvgl-extra/ButtonGroup.h"

#include "Scene.h"

class VideoSettings : public Scene {
    public:
        VideoSettings();
        virtual ~VideoSettings();

        void OnObjectEvent(lv_obj_t* obj, lv_event_t event);
    private:
        ButtonGroup *buttonMatrix[3];
};
