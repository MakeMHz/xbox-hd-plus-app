#pragma once
#include <lvgl.h>
#include "lvgl-extra/ButtonGroup.h"

#include "Scene.h"
#include "XboxConfig.h"

class SystemSettings : public Scene {
    public:
        SystemSettings();
        virtual ~SystemSettings();

        void OnObjectEvent(lv_obj_t* obj, lv_event_t event);
    private:
        void CreateLineBreak();
        lv_obj_t *CreateSubSceneButton(const char *text);

        //
        lv_obj_t *btnSystemAudioSettings;

        ButtonGroup *buttonMatrix[8];

        XboxConfig xboxConfig;
};
