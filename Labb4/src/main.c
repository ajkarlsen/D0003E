#include <stdio.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <unistd.h>
#include "TinyTimber.h"
#include "Pilot.h"
#include "PlsGen.h"
#include "HarbourMaster.h"
#include "Display.h"


HarbourMaster harbourmaster = initHarbourMaster();
Display display = displayInit();
PlsGen gen0 = {initObject(), 0, 0, 4, &harbourmaster, 0}; // Pin 4
PlsGen gen1 = {initObject(), 0, 0, 6, &harbourmaster, 0}; // Pin 6
Pilot pilot = {initObject(), &gen0, &gen1, 0, &display, 0};

void init() {
    LCDCRA = (1 << LCDEN);
		//sätter på displayen
	LCDCCR = (1 << LCDCC0) | (1 << LCDCC1) | (1 << LCDCC2) | (1 << LCDCC3);
					// MAXIMAL STYRKA
	LCDFRR = (0 << LCDPS0) | (0 << LCDPS1) | (0 << LCDPS2); 

    LCDCRB = (1 << LCDCS) | (1 << LCDMUX0) | (1 << LCDMUX1) | (1 << LCDPM0) | (1 << LCDPM1) | (1 << LCDPM2); 
	//button activation
	PORTB = ((1 << PINB7) | (1 << PINB6) | (1 << PINB4));
	// button Left and right
	PORTE = ((1 << PINE2) | (1 << PINE3));
	// interrupt enable
	PCMSK1 = ((1 << 7) | (1 << 6) | (1 << 4));
	PCMSK0 = ((1 << 3) | (1 << 2));
	// External interrupt enable
	EIMSK = ((1 << 7) | (1 << 6));
    
    LCDDR13 = 0x1;
    LCDDR18 = 0x0;
    //set clock
    CLKPR = 0x80;
	CLKPR = 0x00;

};



int main(void) {
    init();


    // Install joystickHandler for both PCINT1 and PCINT0
    INSTALL(&pilot, joystickHandler, IRQ_PCINT1); // Port B
    INSTALL(&pilot, joystickHandler, IRQ_PCINT0); // Port E


    // Initial display update
    //ASYNC(&pilot, updateDisplay, 0);
    return TINYTIMBER(&pilot, updateDisplay, 0);
}