#include "Pilot.h"
#include "TinyTimber.h"
#include <stdbool.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "Display.h"



int joystick(Pilot *self, int direction) {
	PlsGen *active = self->selected == 0 ? self->gen0 : self->gen1;
	switch (direction) {
		case 0: // left
			self->selected = 0;
            ASYNC(self->display, setIndicator, 0);
			break;
		case 1: // right
			self->selected = 1;
            ASYNC(self->display, setIndicator, 1);
			break;
		case 2: // up
			ASYNC(active, incFrequency, 0);
			break;
		case 3: // down
			ASYNC(active, decFrequency, 0);
			break;
		case 4: // press
			if (active->frequency == 0)	{
				ASYNC(active, setFrequency, active->savedFrequency);
			} else {
                active->savedFrequency = active->frequency;
				ASYNC(active, setFrequency, 0);
			}
			break;
	}
	return 0;

}

int clearBounce(Pilot *self, int unused) {
    self->bouncing = 0; 
    return 0;
}

int joystickHandler(Pilot *self, int unused) {
    if (self->bouncing) {
        return 0; 
    }

    self->bouncing = 1; 
    AFTER(MSEC(150), self, clearBounce, 0); 

    if (!(PINB & (1 << 6))) { // Up
        ASYNC(self, joystick, 2);
    } else if (!(PINB & (1 << 7))) { // Down
        ASYNC(self, joystick, 3);
    } else if (!(PINB & (1 << 4))) { // Press
        ASYNC(self, joystick, 4);
    } else if (!(PINE & (1 << 2))) { // Left
        ASYNC(self, joystick, 0);
    } else if (!(PINE & (1 << 3))) { // Right
        ASYNC(self, joystick, 1);
    }
    return 0;
}

int liftOff(Pilot *self, int unused) {
	ASYNC(self->gen0, setFrequency, 0);
    ASYNC(self->gen1, setFrequency, 0);
    
    ASYNC(self->display, setIndicator, 0);
    
    return 0;
}