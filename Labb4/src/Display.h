#ifndef DISPLAY_H
#define DISPLAY_H
#include "TinyTimber.h"

typedef struct {
    Object super;
} Display;

#define displayInit() { initObject() }
int printAt(Display *self, int arg);
void writeChar(char ch, int pos);
#endif