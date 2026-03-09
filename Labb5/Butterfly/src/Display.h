#ifndef DISPLAY_H
#define DISPLAY_H
#include "TinyTimber.h"

typedef struct {
    Object super;
} Display;

#define displayInit() { initObject() }
int printAt(Display *self, int arg);
int setIndicator(Display *self, int active_gen);
#endif