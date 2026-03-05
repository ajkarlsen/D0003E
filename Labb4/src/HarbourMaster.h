#ifndef HARBOURMASTER_H
#define HARBOURMASTER_H

#include "TinyTimber.h"

typedef struct {
    Object super;
} HarbourMaster;

#define initHarbourMaster() { initObject() }

int pinOn(HarbourMaster *self, int pin);
int pinOff(HarbourMaster *self, int pin);

#endif