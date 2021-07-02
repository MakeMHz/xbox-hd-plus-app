#include <hal/xbox.h>
#include <SDL.h>
#include <windows.h>
#include "EEPROM.h"
#include "Loader.h"

int main_loader(void)
{
    SDL_GameController *pad = NULL;
    enum BOOT_OPTION boot = BOOT_NONE;

    if(SDL_Init(SDL_INIT_GAMECONTROLLER) != 0)
        return 0;

    if(SDL_NumJoysticks() < 1)
        return 0;

    if((pad = SDL_GameControllerOpen(0)) == NULL)
        return 0;

    // TODO: Clean up
    for(uint32_t loop = 0; loop < 100; loop++) {
        SDL_GameControllerUpdate();

        if(SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_X)) {
            // Combo (X + A) Load configuration app
            if(SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_A)) {
                boot = BOOT_LOAD_APP;
                break;
            }

            // Combo (X + Black) - Clear EEPROM
            if(SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)) {
                boot = BOOT_CLEAR_EEPROM;
                break;
            }

            // TODO: Combo (X + White) - Load pixel bus test

            // TODO: Combo (X + DOWN) - Force 'default video' output
            // TODO: Combo (X + LEFT) - Force 420P output
            // TODO: Combo (X + UP) - Force 720P output
            // TODO: Combo (X + RIGHT) - Force 1080i output

        }

        if(SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_Y)) {
            // Combo (Y + Dpad Up) Load app shortcut 1
            if(SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_UP)) {
                boot = BOOT_LOAD_CUT1;
                break;
            }

            // Combo (Y + Dpad Right) Load app shortcut 2
            if(SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)) {
                boot = BOOT_LOAD_CUT2;
                break;
            }

            // Combo (Y + Dpad Down) Load app shortcut 3
            if(SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_DOWN)) {
                boot = BOOT_LOAD_CUT3;
                break;
            }
            // Combo (Y + Dpad Left) Load app shortcut 4
            if(SDL_GameControllerGetButton(pad, SDL_CONTROLLER_BUTTON_DPAD_LEFT)) {
                boot = BOOT_LOAD_CUT4;
                break;
            }

        }

        Sleep(1);
    }

    if(boot == BOOT_LOAD_APP)
        XLaunchXBE("C:\\xboxhd\\default.xbe");

    if(boot == BOOT_CLEAR_EEPROM) {
        EEPROM *mEEPROM = new EEPROM();

        mEEPROM->clear();
        mEEPROM->save();
        mEEPROM->upload();
    }

    if(boot == BOOT_LOAD_CUT1)
        XLaunchXBE("C:\\xboxhd\\shortcuts\\1.xbe");

    if(boot == BOOT_LOAD_CUT2)
        XLaunchXBE("C:\\xboxhd\\shortcuts\\2.xbe");

    if(boot == BOOT_LOAD_CUT3)
        XLaunchXBE("C:\\xboxhd\\shortcuts\\3.xbe");

    if(boot == BOOT_LOAD_CUT4)
        XLaunchXBE("C:\\xboxhd\\shortcuts\\4.xbe");

    return 0;
}
