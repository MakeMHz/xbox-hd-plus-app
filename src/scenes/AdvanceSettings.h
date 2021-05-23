#pragma once
#include <lvgl.h>
#include "lvgl-extra/ButtonGroup.h"

#include "EEPROM.h"
#include "Scene.h"

class AdvanceSettings : public Scene {
    public:
        AdvanceSettings();
        virtual ~AdvanceSettings();

        void OnObjectEvent(lv_obj_t* obj, lv_event_t event);
    private:
        void CreateLineBreak();
        lv_obj_t *CreateSubSceneButton(const char *text);

        //
        ButtonGroup *buttonMatrix[4];

        //
        lv_obj_t *btnAdvanceInterpolation;
        lv_obj_t *btnColospace;
        lv_obj_t *btnOverscan;

        //
        EEPROM_T mEEPROM;
};
