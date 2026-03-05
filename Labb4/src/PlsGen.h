#ifndef PLSGEN_H
#define PLSGEN_H
#include "TinyTimber.h"
#include "HarbourMaster.h"
#include "Display.h"

typedef struct {
    Object super;
    int frequency;
    int savedFrequency;
    int selectedPin;
    HarbourMaster *harbourMaster;
    int outputState;
    Display *display;
    int displayPos;
} PlsGen;

#define initPlsGen(pin, harbourMaster, display, displayPos) {initObject(), 0, 0, pin, harbourMaster, 0, display, displayPos}
int setFrequency(PlsGen *self, int frequency);
int start(PlsGen *self, int unused);
int stop(PlsGen *self, int unused);
int toggle(PlsGen *self, int unused);
int incFrequency(PlsGen *self, int isRepeat);
int decFrequency(PlsGen *self, int isRepeat);

#endif