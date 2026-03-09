#include "HarbourMaster.h"
#include <avr/io.h>

int pinOn(HarbourMaster *self, int pin) {
    if (pin == 4) {
        PORTE |= (1 << 4); // Set bit 4
    } else if (pin == 6) {
        PORTE |= (1 << 6); // Set bit 6
    }
    return 0; 
}

int pinOff(HarbourMaster *self, int pin) {
    if (pin == 4) {
        PORTE &= 0xEF; // Clear bit 4 EF = 11101111
    } else if (pin == 6) {
        PORTE &= 0xBF; // Clear bit 6 BF = 10111111
    }
    return 0; 
}