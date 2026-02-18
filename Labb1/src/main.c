/*
 * GccApplication1.c
 *
 * Created: 2026-01-21 13:09:43
 * Author : fabia, alex
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
void writeChar(char ch, int pos);
void lcdIni();
uint32_t lcdvalue(char ch, int pos);
void writeLong(long i);
void lcdreset();
int isprime(long n);
void primes();
void blink();
void button();
void combined();

const uint16_t HALF_TICK = 15625;
uint16_t NEXT_BLINK = 0;
int BUTTON_PRESSED = 0;
const uint16_t digits[10] = {0x1551, 0x8110, 0x11e1, 0x1191, 0x05b0, 0x14b1, 0x14f1, 0x9004, 0x15f1, 0x15b1}; 


int main(void)
{
	
	lcdIni();
	//blink();
	//primes();
	//button();
	combined();
	while (1)
		{
			
	}
	
}

void lcdIni(){
	LCDCRB = (1 << LCDCS) | (1 << LCDMUX0) | (1 << LCDMUX1) | (1 << LCDPM0) | (1 << LCDPM1) | (1 << LCDPM2); 
	//			Clock cycle		s�tter duty till 1/4						S�tter p� alla segments i lcdn
	LCDCRA = (1 << LCDEN);
		//s�tter p� displayen
	LCDCCR = (1 << LCDCC0) | (1 << LCDCC1) | (1 << LCDCC2) | (1 << LCDCC3);
					// MAXIMAL STYRKA
	LCDFRR = (0 << LCDPS0) | (0 << LCDPS1) | (0 << LCDPS2); 

	CLKPR = 0x80;
	CLKPR = 0x00;


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

void writeLong(long i){

    long val = i; 
	char str[6] = {10, 10, 10, 10, 10, 10}; // Initialize all positions to 'blank' (10)

    //lcdreset();

    int pos = 5;

    if (val == 0) {
        writeChar('0', pos); // If value is 0, write 0
    } 

	while (val > 0 && pos >= 0) {
		str[pos] = (val % 10) + '0';
		val /= 10;
		pos--;
	}

	for (int j = 0; j < 6; j++) {
		writeChar(str[j], j);
	}
}

void primes() {

	long value = 0;
	
	while(1) {
		if (isprime(value)) {
			writeLong(value);
		}
		value += 1;
	}
	
}

int isprime(long n) {
	if ( n < 2) return 0;
	for (long i = 2; i <= n / 2; i++) {
		if (n % i == 0) {
			return 0; // Not a prime number
		}
	}
	return 1; // Is a prime number	

}

 void blink() {
	LCDDR18 = 0x1;
	TCCR1B = (0 << CS10) | (0 << CS11) | (1 << CS12); // Prescaler 256

	// (8 MHz)/256 = 31250. This is the clock speed divided by prescaler (256)

	// This is the interval which we want to blink the LED
	const uint16_t half_tick = 15625;
	
	// States next time the LED will blink
	uint16_t next_switch = TCNT1 + half_tick;

	while(1) {
		while ((int16_t)(TCNT1 - next_switch) < 0) {
			// Wait until it's time to switch
			;
		}
		LCDDR3 ^= 0x1; 
		next_switch += half_tick;
	}
}
	 
void button(){

    
    PORTB = (1 << PB7); 

	LCDDR13 = 0x0;
	LCDDR18 = 0x1;

	while(1) {
		while(PINB & (1 << PB7)) {
			;
		}

		LCDDR18 ^= 0x1;
		LCDDR13 ^= 0x1;

		while(!(PINB & (1 << PB7)))
		;
	}
}

// Set all the necessary registers
void init_comb() {
	// Blink functionality
	LCDDR3 = 0x1;
	TCCR1B = (0 << CS10) | (0 << CS11) | (1 << CS12);
	NEXT_BLINK = TCNT1 + HALF_TICK;

	// Button functionality
	PORTB = (1 << PB7); 
	LCDDR18 = 0x0;
	LCDDR13 = 0x1;
	
}

// Part of the blink function but the loop is gone, just blink iff the timer is correct and when it is called
void check_blink() {
	if ((int16_t)(TCNT1-NEXT_BLINK) < 0) {
		LCDDR3 ^= 0x1; 
		NEXT_BLINK += HALF_TICK;
	}
}
// Button but without the loop,
void check_button(){
	int pressed = !(PINB & (1 << PB7));

	// Check if pressed NOW and if last state was not pressed
	if (pressed == 1 && BUTTON_PRESSED == 0) {
		LCDDR18 ^= 0x1;
		LCDDR13 ^= 0x1;
		BUTTON_PRESSED = 1;

		
	}
	// Update last state
	else if (pressed == 0) {
		BUTTON_PRESSED = 0;
	}
}


void combined() {
	init_comb();
	long value = 25000;

	while (1) {
		check_blink();
		check_button();

		if (isprime(value)) {
			writeLong(value);
		}
		value += 1;
	}
}
	