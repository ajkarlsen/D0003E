#include "tinythreads.h"
#include <stdbool.h>
#include <stdio.h>
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
const uint16_t digits[10] = {0x1551, 0x8110, 0x11e1, 0x1191, 0x05b0, 0x14b1, 0x14f1, 0x9004, 0x15f1, 0x15b1}; 
static int pp;
//static volatile int pp;

void lcdIni(); 

void writeChar(char ch, int pos); // from lab 1

bool is_prime(long i); // from lab 1

mutex mutx = MUTEX_INIT;


void lcdIni(){
	LCDCRB = (1 << LCDCS) | (1 << LCDMUX0) | (1 << LCDMUX1) | (1 << LCDPM0) | (1 << LCDPM1) | (1 << LCDPM2); 
	//			Clock cycle		sätter duty till 1/4						S�tter p� alla segments i lcdn
	LCDCRA = (1 << LCDEN);
		//sätter på displayen
	LCDCCR = (1 << LCDCC0) | (1 << LCDCC1) | (1 << LCDCC2) | (1 << LCDCC3);
					// MAXIMAL STYRKA
	LCDFRR = (0 << LCDPS0) | (0 << LCDPS1) | (0 << LCDPS2); 

	CLKPR = 0x80;
	CLKPR = 0x00;

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
	lock(&mutx);

    pp = pos;
    writeChar( (num % 100) / 10 + '0', pp);
//	for(volatile int i = 0; i < 1000; i++) {
//	}
    pp++;
    writeChar( num % 10 + '0', pp);

	unlock(&mutx);
}

void computePrimes(int pos) {
    long n;

    for(n = 1; ; n++) {
        if (is_prime(n)) {
            printAt(n, pos);
        }
    }
}

int main() {
    lcdIni();
    spawn(computePrimes, 0);
    computePrimes(3);
}
