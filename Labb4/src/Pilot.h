#ifndef PILOT_H
#define PILOT_H
#include "TinyTimber.h"
#include "PlsGen.h"
#include "Display.h"

typedef struct {
    Object super;
    PlsGen *gen0;
    PlsGen *gen1;
    int selected; // 0 = left, 1 = right
    Display *display;
    int bouncing; 
} Pilot;

#define initPilot(gen0, gen1) {initObject(), gen0, gen1, 0, display, 0}

// direction: 0=left, 1=right, 2=up, 3=down, 4=press
int joystick(Pilot *self, int direction);
int updateDisplay(Pilot *self, int unused);
int joystickHandler(Pilot *self, int unused);
int clearBounce(Pilot *self, int unused);

#endif