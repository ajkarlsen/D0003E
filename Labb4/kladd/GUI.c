#include "Lab1/include/lcd.h"
#include <avr/io.h>
#include "GUI.h"
#include <stdint-gcc.h>

void buttonLeftRight(GUI *self){
	//Left
	if(!(PINE & 0x4)){
		if (self->selected == 1){
			ASYNC(self, changeSelected, 0);
		}
		writeTwoDigits(self->gen0.frequency, 1);
		//writeTwoDigits(self->gen0.savedfreq, 4);
		toggleSegment(0);
	}
	
	//Right
	if(!(PINE & 0x8)){
		if(self->selected == 0){
			ASYNC(self, changeSelected, 1);
		}
		writeTwoDigits(self->gen1.frequency, 5);
		//writeTwoDigits(self->gen1.savedfreq, 4);
		toggleSegment(4);
	}
}

void buttonUpDown(GUI *self) {
	//upp
	if(!(PINB & 0x40)){
		if(self->selected == 0){
			ASYNC(&self->gen0, incFrequency, &self->gen0);
		}else{
			ASYNC(&self->gen1, incFrequency, &self->gen1);
		}
	}
	
	//down
	if(!(PINB & 0x80)){
		if(self->selected == 0){
			ASYNC(&self->gen0, decFrequency, &self->gen0);
		}else{
			ASYNC(&self->gen1, decFrequency, &self->gen1);
		}
	}
	
	//enter (save)
	if (!(PINB & 0x10)){
		if(self->selected == 0){
			if(self->gen0.frequency == 0){
				ASYNC(&self->gen0, restore, &self->gen0);
			}else{
				ASYNC(&self->gen0, saveFrequency, &self->gen0);
				ASYNC(&self->gen0, reset, &self->gen0);
			}
		}else{
			if(self->gen1.frequency == 0){
				ASYNC(&self->gen1,restore, &self->gen1);
			}else{
				ASYNC(&self->gen1,saveFrequency, &self->gen1);
				ASYNC(&self->gen1,reset, &self->gen1);
			}
		}		
	}	
}


void changeSelected(GUI *self, int newSelected){
	self->selected = newSelected;
}

void Interrupt(GUI *self){
	ASYNC(&self->gen0, run, NULL);
	ASYNC(&self->gen1, run, NULL);
}


