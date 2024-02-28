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

    // TODO: Clean up
    for(uint32_t loop = 0; loop < 100; loop++) {
        SDL_GameControllerUpdate();

        SDL_Event e;
        while(SDL_PollEvent(&e)) {
            if(e.type == SDL_CONTROLLERDEVICEADDED) {
                SDL_GameController *new_pad = SDL_GameControllerOpen(e.cdevice.which);
                if (pad == NULL) {
                    pad = new_pad;
                }
            }
            else if (e.type == SDL_CONTROLLERDEVICEREMOVED) {
                if (pad == SDL_GameControllerFromInstanceID(e.cdevice.which)) {
                    pad = NULL;
                }
                SDL_GameControllerClose(SDL_GameControllerFromInstanceID(e.cdevice.which));
            }
            else if (e.type == SDL_CONTROLLERBUTTONDOWN) {
                if (e.cbutton.button == SDL_CONTROLLER_BUTTON_START) {
                    pad = (SDL_GameControllerFromInstanceID(e.cdevice.which));
                }
            }
        }

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

        Sleep(1);
    }

    if(boot == BOOT_LOAD_APP)
        XLaunchXBE("C:\\xboxhd\\default.xbe");

    if(boot == BOOT_CLEAR_EEPROM) {
        EEPROM *mEEPROM = new EEPROM();

        mEEPROM->clear();
        mEEPROM->save();

        HalReturnToFirmware(HalRebootRoutine);
    }

    return 0;
}
