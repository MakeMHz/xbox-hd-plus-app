#pragma once
#include <lvgl.h>
#include "lvgl-extra/ButtonGroup.h"
#include "lvgl-extra/SwitchLabel.h"

#include "Scene.h"

class VideoSettings : public Scene {
    public:
        VideoSettings();
        virtual ~VideoSettings();

        void OnObjectEvent(lv_obj_t* obj, lv_event_t event);
    private:
        ButtonGroup *buttonMatrix[2];
        SwitchLabel *btnPrescaleCorrection;
        SwitchLabel *btnDisable480p16X9;

        // Help panel objects
        void UpdateHelperText();
        lv_obj_t *contHelp;
        lv_obj_t *contHelpLabel;
};
