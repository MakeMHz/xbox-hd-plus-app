#pragma once
#include <stdio.h>
#include <string.h>
#include <memory>
#include <lvgl.h>

class Scene {
    public:
        Scene();
        virtual ~Scene() { };

        void DrawBackButton();
        void DrawBackButton(const char *text);

        lv_obj_t *parent_screen;
        lv_obj_t *screen;
        lv_group_t *group;
        lv_obj_t *cont;
    protected:
        std::unique_ptr<Scene> parent;
        std::unique_ptr<Scene> child;
    private:

        lv_obj_t *back_button_image = NULL;
        lv_obj_t *back_button_label = NULL;
};

enum SCENE {
    ROOT,
    SYSTEM_SETTINGS,
    SYSTEM_AUDIO_SETTINGS,
    ADVANCED_SETTINGS,
    USER_MANUAL,
    INTERPOLATION_SETTINGS,
    SELF_TEST,
    VIDEO_SETTINGS,
    CREDITS,
    COLOR_CORRECTION,
    OVERSCAN_CORRECTION
};
