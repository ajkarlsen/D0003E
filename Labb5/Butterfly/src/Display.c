#include "Display.h"
#include "BridgeController.h" // We need this to access the bridge variables
#include <avr/io.h>

const uint16_t digits[10] = {0x1551, 0x8110, 0x11e1, 0x1191, 0x05b0, 0x14b1, 0x14f1, 0x9004, 0x15f1, 0x15b1};

// Helper to write a single digit
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

// Helper to write a 2-digit number at a starting position
void printAt(int pos, int num) {
    writeChar(((num / 10) % 10) + '0', pos);
    writeChar((num % 10) + '0', pos + 1);
}

void Update_LCD_Event(Display *self, int bridge_ptr) {
    BridgeController *bridge = (BridgeController *)bridge_ptr;

    printAt(0, bridge->north_queue);
    
    printAt(4, bridge->south_queue);
    
    printAt(2, bridge->cars_on_bridge);
}