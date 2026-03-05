

#ifndef PULSGEN_H_
#define PULSGEN_H_

#include "TinyTimber.h"
#include "PortController.h"

typedef struct {
	Object super;
	int frequency;
	int savedFrequency;
	int selectedPin;
	PortController *portController;
	
} PulseGen;

void run(PulseGen *self);
void saveFrequency(PulseGen *self);
void reset(PulseGen *self);
void incFrequency(PulseGen *self);
void decFrequency(PulseGen *self);
void restore(PulseGen *self);

#define initPulsGen(selectedPin, portController){initObject(), 0,0, selectedPin, portController}

#endif /* PULSGEN_H_ */