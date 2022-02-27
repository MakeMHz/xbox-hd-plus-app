#pragma once
#include <lvgl.h>
#include "lvgl-extra/ButtonGroup.h"

#include "Scene.h"
#include "Update.h"

class SceneRoot : public Scene {
    public:
        SceneRoot();
        virtual ~SceneRoot();

        lv_obj_t *CreateSubSceneButton(const char *text);
        void OnObjectEvent(lv_obj_t* obj, lv_event_t event);
    private:
        //
        lv_obj_t *btnSelfTest;
        lv_obj_t *btnSystemSettings;
        lv_obj_t *btnVideoSettings;
        lv_obj_t *btnAdvanceSettings;
        lv_obj_t *btnFirmware;
        lv_obj_t *btnUserManual;
        lv_obj_t *btnAbout;
};
