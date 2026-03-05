#include <stdbool.h>
#include <stdint.h>
#include <avr/io.h>
#include "include/lcd.h"


#define F_CPU         8000000UL				// CPU frequency 8 MHz
#define PRESCALER     256
#define TIMER_FREQ    (F_CPU / PRESCALER)	// 31,250 Hz
#define HALF_PERIOD   (TIMER_FREQ / 2)			// 15,625 for a 0.5-second delay


static uint16_t next_toggle_time = 0; 



void blink_init() {
	TCCR1B = 1 << CS12;							// Prescalar -> 256
	next_toggle_time = TCNT1 + HALF_PERIOD;		// Set first toggle time
}


void blink(void) {
	
	blink_init();

	while (1) {
		while ( (uint16_t)(TCNT1 - next_toggle_time) < 0x8000 ) {	// Waits if the time haven't passed		
			// Busy waiting
		}
		LCDDR2 ^= 0x6;

		next_toggle_time += HALF_PERIOD;
	}
}

void blink4() {
	uint16_t current_time = TCNT1;
	
	if ((uint16_t)(current_time - next_toggle_time) < 0x8000) {return;}		// Returns if the time haven't passed
	
	LCDDR2 ^= 0x6;		// Toggle segment

	next_toggle_time += HALF_PERIOD;	// Set next toggle
}