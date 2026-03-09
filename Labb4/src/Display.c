#include "Display.h"
#include <avr/io.h>

const uint16_t digits[10] = {0x1551, 0x8110, 0x11e1, 0x1191, 0x05b0, 0x14b1, 0x14f1, 0x9004, 0x15f1, 0x15b1};

void writeChar(char ch, int pos) {
    uint16_t a = 0;
    if (ch >= '0' && ch <= '9') a = digits[ch - '0'];
    else if (ch >= 0 && ch <= 9) a = digits[ch];

    volatile uint8_t *base = &LCDDR0;
    int offset = (pos / 2);
    int isOdd = (pos % 2);
    volatile uint8_t *ptr = base + offset;

    for (int i = 0; i < 4; i++) {
        uint8_t nibble = (a >> (12 - (i*4))) & 0x0F;
        volatile uint8_t *target = ptr + (i*5);
        if (isOdd) *target = (*target & 0x0F) | (nibble << 4);
        else *target = (*target & 0xF0) | (nibble);
    }
}

int printAt(Display *self, int arg) {

    int pos = arg >> 8;
    int num = arg & 0xFF;
    writeChar(((num / 10) % 10) + '0', pos);
    writeChar((num % 10) + '0', pos + 1);
    return 0;
}

int setIndicator(Display *self, int active_gen) {
    if (active_gen == 0){ 
        LCDDR13 = 1; // Highlight gen0
        LCDDR18 = 0; // Unhighlight gen1
    } else if (active_gen == 1) {
        LCDDR13 = 0; // Highlight gen1
        LCDDR18 = 1; // Unhighlight gen0
    }
    return 0;
}