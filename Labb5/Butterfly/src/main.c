#include <avr/io.h>
#include <avr/interrupt.h>
#include "TinyTimber.h"
#include "BridgeController.h"
#include "Display.h"
#include "Reciever.h"

// --- Constants matching your Mac Simulator ---
#define SOUTH 0
#define NORTH 1
#define RED 2

#define NorthQueueArrival 1
#define NorthBridgeArrival 2
#define SouthQueueArrival 4
#define SouthBridgeArrival 8

#define NorthGreen 9
#define SouthGreen 6
#define RedRed 10

// --- 1. The Reactive Object ---
// This struct holds the entire state of your bridge


// Instantiate your bridge
Display display = initDisplay();
BridgeController bridge = initBridgeController(&display);
Reciever reciever = initReciever(&bridge);

// --- 2. Hardware Initialization ---
void Init(void) {
    // 1. Clock Config
    CLKPR = 0x80;
    CLKPR = 0x00;

    // 2. LCD Config (from your previous code)
    LCDCRA = (1 << LCDEN);
    LCDCCR = (1 << LCDCC0) | (1 << LCDCC1) | (1 << LCDCC2) | (1 << LCDCC3);
    LCDFRR = (0 << LCDPS0) | (0 << LCDPS1) | (0 << LCDPS2); 
    LCDCRB = (1 << LCDCS) | (1 << LCDMUX0) | (1 << LCDMUX1) | (1 << LCDPM0) | (1 << LCDPM1) | (1 << LCDPM2); 

    // 3. USART Config (9600 Baud)
    unsigned int ubrr = 8000000UL/16/9600-1;
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    
    // IMPORTANT: We added (1<<RXCIE0) here to enable the Receive Interrupt!
    UCSR0B = (1<<RXEN0) | (1<<TXEN0) | (1<<RXCIE0);
    UCSR0C = (1<<UCSZ01) | (1<<UCSZ00); 
}

// Function to safely send a byte back to the Mac
void Serial_Write(unsigned char data) {
    while (!(UCSR0A & (1<<UDRE0))); // Wait for empty transmit buffer
    UDR0 = data;
}


int main(void) {
    Init();
    
    // Tell TinyTimber to route the USART Receive Interrupt to our Receive_Sensor_Data method
    // Note: IRQ_USART0_RX is the standard TinyTimber AVR vector name. 
    INSTALL(&reciever, Receive_Data, IRQ_USART0_RX);
    
    // Boot the TinyTimber kernel and call Start_Bridge
    return TINYTIMBER(&bridge, Start_Bridge, 0);
}