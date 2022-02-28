#pragma once
#include <lvgl.h>
#include "lvgl-extra/ButtonGroup.h"

#include "Scene.h"
#include "XboxConfig.h"

class SystemAudioSettings : public Scene {
    public:
        SystemAudioSettings();
        virtual ~SystemAudioSettings();

        void OnObjectEvent(lv_obj_t* obj, lv_event_t event);
    private:
        void CreateLineBreak();
        lv_obj_t *CreateSubSceneButton(const char *text);

        //
        lv_obj_t *btnSystemAudioSettings;
        ButtonGroup *buttonMatrix[3];

        XboxConfig xboxConfig;

        // Help panel objects
        lv_obj_t *contHelp;
        lv_obj_t *contHelpLabel;
};
