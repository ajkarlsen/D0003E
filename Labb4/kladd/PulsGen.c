#include "TinyTimber.h"
#include "GUI.h"
#include "Lab1/include/lcd.h"
#include <avr/io.h>
#include "PortController.h"
#include "PulsGen.h"



void run(PulseGen *self){
	if(self->frequency != 0){
		if(self->selectedPin == 1){ //right
			ASYNC(self->portController, togglePin6, NULL);
		}else{ //left
			ASYNC(self->portController, togglePin4, NULL);
		}
		AFTER(MSEC(500)/self->frequency, self, run, NULL);
	}else{
		if(self->selectedPin == 1){//right
			ASYNC(self->portController, disablePin6, NULL);
		}else{//left
			ASYNC(self->portController, disablePin4, NULL);
		}
		AFTER(MSEC(300), self, run, NULL);
	}
}

void saveFrequency(PulseGen *self){
	self->savedFrequency = self->frequency;
	//writeTwoDigits(self->savedFrequency, 2);
}

void reset(PulseGen *self){
	self->frequency = 0;
	//left
	if(self->selectedPin == 0){ 
		writeTwoDigits(self->frequency, 1);
		//toggleSegment(0);
	}else{ //right
		writeTwoDigits(self->frequency, 5);
		//toggleSegment(4);
	}
}

void incFrequency(PulseGen *self){
	if(self->frequency < 99){
		self->frequency++;
		
		if(self->selectedPin == 0){
			writeTwoDigits(self->frequency,1);
			//toggleSegment(0);
		}else{
			writeTwoDigits(self->frequency,5);
		}
		//holding up
		if(!(PINB & 0x40)){
			AFTER(MSEC(600), self, incFrequency, self);
		}

	}
}

void decFrequency(PulseGen *self){
	if(self->frequency > 0){
		self->frequency--;

		if(self->selectedPin == 0){
			writeTwoDigits(self->frequency,1);
			//toggleSegment(0);
			}else{
			writeTwoDigits(self->frequency,5);
		}
		//holding up
		if(!(PINB & 0x80)){
			AFTER(MSEC(600), self, decFrequency, self);
		}

	}
}

void restore(PulseGen *self){
	self->frequency = self->savedFrequency;
	self->savedFrequency = 0;
	//left
	if(self->selectedPin == 0){
		writeTwoDigits(self->frequency, 1);
		//toggleSegment(0);
	}else{ //right
		writeTwoDigits(self->frequency, 5);
		//toggleSegment(4);
	}
}













