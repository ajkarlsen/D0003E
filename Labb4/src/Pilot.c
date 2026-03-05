#include "Pilot.h"
#include "TinyTimber.h"
#include <stdbool.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "Display.h"

int updateDisplay (Pilot *self, int unused) {
	int freq0 = self->gen0->frequency;
	int freq1 = self->gen1->frequency;
	ASYNC(self->display, printAt, (0 << 8) | freq0);
	ASYNC(self->display, printAt, (4 << 8) | freq1);
	return 0;
}

int joystick(Pilot *self, int direction) {
	PlsGen *active = self->selected == 0 ? self->gen0 : self->gen1;
	switch (direction) {
		case 0: // left
			self->selected = 0;
			break;
		case 1: // right
			self->selected = 1;
			break;
		case 2: // up
			ASYNC(active, setFrequency, active->frequency+1);
			if (!(PINB & (1 << 6))) {
				AFTER(MSEC(300), self, joystick, 2);
			}
			break;
		case 3: // down
			if (active->frequency > 0 ) {
				ASYNC(active, setFrequency, active->frequency-1);
			}
			if (!(PINB & (1 << 7))) {
				AFTER(MSEC(300), self, joystick, 3);
			}
			break;
		case 4: // press
			if (active->frequency == 0)	{
				ASYNC(active, setFrequency, active->savedFrequency);
			} else {
				ASYNC(active, setFrequency, 0);
			}
	}
	ASYNC(self, updateDisplay, 0);
	return 0;

	
}

int joystickHandler(Pilot *self, int unused) {
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

int holdUp(Pilot *self, int unused) {
    if (!(PINB & (1 << 6))) { 
        ASYNC(self, joystick, 2); // Send the UP command
        AFTER(MSEC(1000), self, holdUp, 0); // Schedule another check in 200ms
    }
    return 0;
}

int holdDown(Pilot *self, int unused) {
    if (!(PINB & (1 << 7))) { 
        ASYNC(self, joystick, 3); 
        AFTER(MSEC(1000), self, holdDown, 0); 
    }
    return 0;
}