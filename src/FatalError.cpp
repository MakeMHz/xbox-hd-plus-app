
#include <lvgl.h>
#include <lv_sdl_drv_display.h>
#include <lv_sdl_drv_input.h>
#include <hal/xbox.h>
#include <windows.h>

void DisplayFatalError(const char *message) {
    static const char *btns[] = { "Reboot", "" };

    // Register input device
    lv_indev_t *sdl_indev = lv_indev_get_next(NULL);
    lv_group_t *group = lv_group_create();
    if(sdl_indev != NULL)
        lv_indev_set_group(sdl_indev, group);

    lv_obj_t *mbox1 = lv_msgbox_create(lv_scr_act(), NULL);
    lv_msgbox_set_text_fmt(mbox1, message);

    lv_msgbox_add_btns(mbox1, btns);
    lv_obj_set_width(mbox1, 400);
    lv_group_add_obj(group, mbox1);
    lv_obj_align(mbox1, NULL, LV_ALIGN_CENTER, 0, 0);

    while(!get_quit_event()) {
        if(lv_msgbox_get_active_btn(mbox1)) {
            HalReturnToFirmware(HalRebootRoutine);
        }

        lv_task_handler();
        Sleep(15);
    }
}
