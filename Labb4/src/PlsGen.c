#include "PlsGen.h"
#include "avr/io.h"
#include "TinyTimber.h"

int setFrequency(PlsGen *self, int freq) {
    int oldFreq = self->frequency;
    self -> frequency = freq;

    if (freq == 0) {
        ASYNC(self->harbourMaster, pinOff, self->selectedPin);
    } else {
        self -> savedFrequency = freq;
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
