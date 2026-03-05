#ifndef PLSGEN_H
#define PLSGEN_H
#include "TinyTimber.h"
#include "HarbourMaster.h"

typedef struct {
    Object super;
    int frequency;
    int savedFrequency;
    int selectedPin;
    HarbourMaster *harbourMaster;
    int outputState;
} PlsGen;

#define initPlsGen(pin, harbourMaster) {initObject(), 0, 0, pin, harbourMaster, 0}

int setFrequency(PlsGen *self, int frequency);
int start(PlsGen *self, int unused);
int stop(PlsGen *self, int unused);
int toggle(PlsGen *self, int unused);

#endif