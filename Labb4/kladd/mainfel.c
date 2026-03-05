

#include <avr/io.h>
#include <avr/delay.h>
#include <unistd.h>

#include "Lab1/include/lcd.h"
#include "TinyTimber.h"
#include "GUI.h"
#include "PulsGen.h"
#include "PortController.h"

void disable_prescaler() {
	CLKPR = 0x80;
	CLKPR = 0x00;
}


void initJoy(){
	//enabels pul up resistors , (down, upp, enter)
	
	PORTB = ((1 << PINB7) | (1 << PINB6) | (1 << PINB4));
	//left, right?
	PORTE = ((1 << PINE2) | (1 << PINE3));
	
	//Enabels interupt? , (down, upp, enter)
	PCMSK1 = ((1 << 7) | (1 << 6) | (1 << 4));
	PCMSK0 = ((1 << 3) | (1 << 2));
	
	//EIFR = ((1 << 7) | (1 << 6));
	EIMSK = ((1 << 7) | (1 << 6));
}

void initClock(){
	TCCR1B = 0xD;
	OCR1A = 3906;
	TIMSK1 = (1<<OCIE1A);
	TCNT1 = 0;
}


int main(void) {
	PortController p =initPortController();
	PulseGen gen0 = initPulsGen(0, &p);
	PulseGen gen1 = initPulsGen(1, &p);
	GUI gui = initGUI(gen0, gen1);
	
	disable_prescaler();
	initJoy();
	LCD_Init();
	
	toggleSegment(0);
	
	writeTwoDigits(0, 1);
	writeTwoDigits(0, 5);
	
	INSTALL(&gui, buttonLeftRight, IRQ_PCINT0);
	INSTALL(&gui, buttonUpDown, IRQ_PCINT1);
	return TINYTIMBER(&gui, Interrupt, NULL);
}

