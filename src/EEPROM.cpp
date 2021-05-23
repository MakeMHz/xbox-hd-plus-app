#include <stdio.h>
#include <string.h>

#include "EEPROM.h"

EEPROM::EEPROM() { }

// Store 'EEPROM' to file
void EEPROM::set(EEPROM_T e)
{
    current = e;
}

// Load in 'EEPROM'
EEPROM_T EEPROM::get()
{
    current.colorspace = COLORSPACE::RGB;
    return current;
}
