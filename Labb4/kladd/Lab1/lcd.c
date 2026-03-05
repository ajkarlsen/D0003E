#include <avr/io.h>
#include <stdint.h>
#include <stdbool.h>

#define odd_pos_mask 0x0F
#define even_pos_mask 0xF0




uint8_t scc_table[10][4] = {    // Bit mapping for integers 0-9
	{0x1, 0x5, 0x5, 0x1},   // 0
	{0x0, 0x1, 0x1, 0x0},   // 1
	{0x1, 0x1, 0xE, 0x1},   // 2
	{0x1, 0x1, 0xB, 0x1},   // 3
	{0x0, 0x5, 0xB, 0x0},   // 4
	{0x1, 0x4, 0xB, 0x1},   // 5
	{0x1, 0x4, 0xF, 0x1},   // 6
	{0x1, 0x1, 0x1, 0x0},   // 7
	{0x1, 0x5, 0xf, 0x1},   // 8
	{0x1, 0x5, 0xB, 0x1}	// 9
};

volatile uint8_t* lcd_reg[][4] = {                // Group LCDDRx registers
	{&LCDDR0, &LCDDR5, &LCDDR10, &LCDDR15},       // Digits 0-1
	{&LCDDR1, &LCDDR6, &LCDDR11, &LCDDR16},       // Digits 2-3
	{&LCDDR2, &LCDDR7, &LCDDR12, &LCDDR17}        // Digits 4-5
};

void LCD_Init(void) {
	LCDCRA |= 0xC0;		// LCD Enable, Low power Waveform
	LCDCRB |= 0x37;		// 1/3 Bias, 1/4 Duty, SEG0:24
	LCDFRR |= 0x7;		// Prescaler N=16, Clock divide D=8
	LCDCCR |= 0xF;		// Drivetime 300 microseconds, Voltage 3,35 V
}


void writeChar(char ch, int pos){
	int8_t font_idx = ch - 48;        // ASCII table offset is -48

	// Check if arguments are in range
	if (font_idx < 0 || font_idx > 9) {return;}
	if (pos > 5 || pos < 0) {return;}

	uint8_t odd_pos = (pos % 2 != 0) ? 1 : 0;	// Check if pos is odd
	pos = pos >> 1;								// Floor division with 2 to make lookup work properly


	for (int i = 0; i < 4; i++) {
		uint8_t current_value = *lcd_reg[pos][i];	// Reads current value of LCDDR register
		uint8_t value = scc_table[font_idx][i];			// Fetch value to write into register

		if (odd_pos) {value = value << 4;}									// Shift value to upper nibble
		uint8_t bit_mask = (odd_pos) ? (odd_pos_mask) : (even_pos_mask);	// Sets bit mask depending on odd/even pos
		
		if (i == 0) {bit_mask = (odd_pos) ? (0x6F) : (0xF6);}	// Hinders non digit segments in LCDDR0:2 to be overwritten
		uint8_t masked_value = current_value & bit_mask;	// Applies bit mask on current value

		*lcd_reg[pos][i] = masked_value | value;	// Write to LCDDR register
	};
}

void writeLong(long i) {
	int8_t k = 5;
	
	while (k >= 0 && i > 0) {
		
		uint8_t digit = i % 10;
		i /= 10;
		
		char ch = digit + 48;		// ASCII table offset is 48
		
		writeChar(ch, k);
		
		k--;
	}
}


void writeTwoDigits(long i, uint8_t pos) {
	int8_t k = pos;
	while (k >= (pos - 1)) {
		uint8_t digit = i % 10;
		i /= 10;
		char ch = digit + 48;		// ASCII table offset is 48
	
		writeChar(ch, k);
		k--;
	}
}

void toggleSegment(int pos) {
	if (pos == 0) {
		LCDDR1 &= ~(0x40);
		LCDDR0 |= 0x2;
	}
	if (pos == 4) {
		LCDDR0 &= ~(0x2);
		LCDDR1 |= 0x40;
	}
	
}
// LCDDR0 = 0x2;
// LCDDR1 = 0x40;