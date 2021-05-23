#include <stdio.h>
#include <string.h>

#include "EEPROM.h"

EEPROM::EEPROM() { }

void EEPROM::set(EEPROM_T e)
{
    current = e;
}

EEPROM_T EEPROM::get()
{
    current.colorspace = COLORSPACE::RGB;
    return current;
}
