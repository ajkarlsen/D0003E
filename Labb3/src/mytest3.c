#include "tinythreads.h"
#include <stdbool.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
const uint16_t digits[10] = {0x1551, 0x8110, 0x11e1, 0x1191, 0x05b0, 0x14b1, 0x14f1, 0x9004, 0x15f1, 0x15b1}; 
static long KEYPRESSED = 0;

void Init(); 
void writeChar(char ch, int pos); // from lab 1
bool is_prime(long i); // from lab 1
void button();
void blink();

mutex blink_mutex = {1,0};  
mutex button_mutex = {1, 0};



void Init(){
	LCDCRB = (1 << LCDCS) | (1 << LCDMUX0) | (1 << LCDMUX1) | (1 << LCDPM0) | (1 << LCDPM1) | (1 << LCDPM2); 
	//			Clock cycle		sätter duty till 1/4						S�tter p� alla segments i lcdn
	LCDCRA = (1 << LCDEN);
		//sätter på displayen
	LCDCCR = (1 << LCDCC0) | (1 << LCDCC1) | (1 << LCDCC2) | (1 << LCDCC3);
					// MAXIMAL STYRKA
	LCDFRR = (0 << LCDPS0) | (0 << LCDPS1) | (0 << LCDPS2); 

	CLKPR = 0x80; // CLock to 8MHz
	CLKPR = 0x00;

	TCNT1 = 0; // Reset timer count
    OCR1A = 3906; // (8 000 000 / 1024) * 0,5 = 3906.25 --> 500ms interrupt interval
    TIMSK1 = (1 << OCIE1A); 
    TCCR1B = (1 << WGM12) | (1 << CS10) | (0 << CS11) | (1 << CS12);
    TCCR1A = (1 << COM1A1) | (1 << COM1A0);


	PORTB = (1 << PB7); 
    PCMSK1 = (1 << PCINT15); 
    EIMSK = (1 << PCIE1);
}


ISR(TIMER1_COMPA_vect) {
    spawn(blink, 0); 
}

ISR(PCINT1_vect) {
    int pressed = !(PINB & (1 << PB7));
	if (pressed == 1) {
		spawn(button, 0);
	}
}

bool is_prime(long n) {
	if ( n < 2) return 0;
	for (long i = 2; i <= n / 2; i++) {
		if (n % i == 0) {
			return 0; // Not a prime number
		}
	}
	return 1; // Is a prime number	

}

void writeChar(char ch, int pos){
	uint16_t a = 0; 

	if (ch >= '0' && ch <= '9') {
		a = digits[ch - '0'];
	}
	else if (ch >= 0 && ch <= 9) {
		a = digits[ch];
	}

	// Declare pointer to LCDDR0
	volatile uint8_t *base = &LCDDR0; 

	int offset = (pos / 2);
	int isOdd = (pos % 2);
	
	volatile uint8_t *ptr = base + offset;


	for (int i = 0; i<4; i++) {
		// Rotating with 12 --> 4 bits depending on the register
		uint8_t nibble = (a >> (12 - (i*4))) & 0x0F;

		volatile uint8_t *target = ptr + (i*5);

		if (isOdd) {
			*target = (*target & 0x0F) | (nibble << 4);
		}
		else {
			*target = (*target & 0xF0) | (nibble);
		}
	}
}

void printAt(long num, int pos) {

    int pp = pos;
    writeChar( (num % 100) / 10 + '0', pp);

    pp++;
    writeChar( num % 10 + '0', pp);

}

void computePrimes(int pos) {
    long n;

    for(n = 1; ; n++) {
        if (is_prime(n)) {
            printAt(n, pos);
        }
    }
}

void button(){

    
    PORTB = (1 << PB7);

	LCDDR13 = 0x0;
	LCDDR18 = 0x1;


		LCDDR18 ^= 0x1;
		LCDDR13 ^= 0x1;
		
		KEYPRESSED ++;
		printAt(KEYPRESSED, 4);
		
}

void blink() {
		LCDDR3 ^= 0x1; 
}

int main() {
    Init();
    spawn(button, 0);
	spawn(blink, 0);
    computePrimes(0);
}
