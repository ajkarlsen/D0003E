#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "TinyTimber.h"

// Forward declaration of BridgeController since we'll pass a pointer to it
struct BridgeController;

typedef struct {
    Object super;
} Display;

#define initDisplay() { initObject() }

// The method triggered by BridgeController to redraw the screen
void Update_LCD_Event(Display *self, int bridge_ptr);

#endif