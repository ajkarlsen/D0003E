 
#include <stdbool.h>
#include <avr/io.h>
#include <avr/delay.h>
#include "include/lcd.h"

bool PRESSED;
bool SEGMENT_STATE = false;

long inputCounter = 0;


void initialize_io() {
	DDRB &= ~(1 << DDB7);	 // Set PB7 as input
	PORTB |= (1 << PB7);	// Enable pull-up resistor on PB7
}

void incrementCounter() {
	inputCounter++;
}


void button() {
	initialize_io();
	
	while (true) {
		
		if ((PINB >> 7) && 1) {PRESSED = false;}	// Wait for input to be released
			
		if(PRESSED) {continue;}		// Handle long inputs
		
		if (!((PINB >> 7) && 1)) {		// Detect input
			PRESSED = true;
			
			SEGMENT_STATE = !SEGMENT_STATE;		// Toggle state
		}
		_delay_ms(50);
	}
}


void button4() {
	if ((PINB >> 7) && 1) {PRESSED = false;}
		
	if(PRESSED) {return;}
		
	if (!((PINB >> 7) && 1)) {
		PRESSED = true;
		if (SEGMENT_STATE) {
			LCDDR0 |= 0x6;
			LCDDR0 &= ~(0x60);
		}
		else {
			LCDDR0 |= 0x60;
			LCDDR0 &= ~(0x6);
		}
		SEGMENT_STATE = !SEGMENT_STATE;
	}
}