#include "PlsGen.h"
#include "avr/io.h"
#include "TinyTimber.h"
#include "Pilot.h"
extern Pilot pilot;

int setFrequency(PlsGen *self, int freq) {
    int oldFreq = self->frequency;
    self -> frequency = freq;

    if (freq == 0) {
        ASYNC(self->harbourMaster, pinOff, self->selectedPin);
    } else {
        if (oldFreq == 0) {
            ASYNC(self, start, 0);
        }
    }
    return 0;
    
}


int start(PlsGen *self, int unused) {
    if (self->frequency > 0) {
        self -> outputState = 1;
        ASYNC(self->harbourMaster, pinOn, self->selectedPin);
        
        if (self->frequency == 0) {
            ASYNC(self->harbourMaster, pinOff, self->selectedPin); 
        }else {
            int half_time = 500 / self->frequency;
            AFTER(MSEC(half_time), self, toggle, 0);
        }
        
    }
    return 0;
}

int stop(PlsGen *self, int unused) {
    self -> outputState = 0;
    ASYNC(self->harbourMaster, pinOff, self->selectedPin);
    return 0;
}

int toggle(PlsGen *self, int unused) {
    if (self->frequency == 0) {
        return 0; // Do nothing if frequency is zero
    }
    
    if (self->outputState) {
        ASYNC(self->harbourMaster, pinOff, self->selectedPin);
        self->outputState = 0;
    } else {
        ASYNC(self->harbourMaster, pinOn, self->selectedPin);
        self->outputState = 1;
    }

    int half_period_ms = 500 / self->frequency; 
    AFTER(MSEC(half_period_ms), self, toggle, 0);
    
    return 0;
}

int incFrequency(PlsGen *self, int is_repeat) {
    // 1. If this is a repeat loop, check the button FIRST.
    // If the UP button (PINB6) is NOT pressed, stop the loop immediately!
    if (is_repeat == 1 && (PINB & (1 << 6))) {
        return 0; 
    }

    // 2. We are safe to increment
    if (self->frequency < 99) {
        ASYNC(self, setFrequency, self->frequency + 1);
        ASYNC(&pilot, updateDisplay, 0); // See Fix 2 below
    }

    if (is_repeat == 0) {
        AFTER(MSEC(500), self, incFrequency, 1); 
    } else {
        AFTER(MSEC(150), self, incFrequency, 1); 
    }
    return 0;
}

int decFrequency(PlsGen *self, int is_repeat) {
    if (is_repeat == 1 && (PINB & (1 << 7))) {
        return 0;
    }

    if (self->frequency > 0) {
        ASYNC(self, setFrequency, self->frequency - 1);
        ASYNC(&pilot, updateDisplay, 0);
    }

    if (is_repeat == 0) {
        AFTER(MSEC(500), self, decFrequency, 1);
    } else {
        AFTER(MSEC(150), self, decFrequency, 1);
    }
    return 0;
}
