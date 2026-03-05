#ifndef GUI_H_
#define GUI_H_

#include "pulsGen.h"


typedef struct {
	Object super;
	PulseGen gen0;
	PulseGen gen1;
	int selected;
	}GUI;


#define initGUI(gen0, gen1) {initObject(), gen0, gen1, 0} 

void buttonLeftRight(GUI *gui);
void buttonUpDown(GUI *gui);
void changeSelected(GUI *self, int newSelected);
void Interrupt(GUI *self);



#endif /* GUI_H_ */