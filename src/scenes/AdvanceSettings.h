#pragma once
#include <lvgl.h>
#include "lvgl-extra/ButtonGroup.h"
#include "lvgl-extra/SwitchLabel.h"

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
        ButtonGroup *buttonMatrix[3];
        SwitchLabel *btnAutoVideoBlanking;
        SwitchLabel *btnAutoRegionSwitch;

        //
        lv_obj_t *btnAdvanceInterpolation;
        lv_obj_t *btnLoadOptimizedSettings;

        //
        EEPROM_T mEEPROM;

        // Help panel objects
        void UpdateHelperText();
        lv_obj_t *contHelp;
        lv_obj_t *contHelpLabel;
};
