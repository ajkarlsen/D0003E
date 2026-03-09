#include <avr/io.h>

// Assume standard 8MHz clock for Butterfly. If your terminal outputs 
// garbage characters later, your board might be running at 2MHz or 1MHz.
#define F_CPU 8000000UL 
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

void USART_Init(unsigned int ubrr) {
    // Set baud rate
    UBRR0H = (unsigned char)(ubrr>>8);
    UBRR0L = (unsigned char)ubrr;
    
    // Enable receiver and transmitter
    UCSR0B = (1<<RXEN0)|(1<<TXEN0);
    
    // Set frame format: 8 data bits, no parity, 1 stop bit (9600 8N1)
    UCSR0C = (1<<UCSZ01)|(1<<UCSZ00); 
}

void USART_Transmit(unsigned char data) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1<<UDRE0))) ;
    // Put data into buffer, sends the data
    UDR0 = data;
}

unsigned char USART_Receive(void) {
    // Wait for data to be received
    while (!(UCSR0A & (1<<RXC0))) ;
    // Get and return received data from buffer
    return UDR0;
}

int main(void) {
    USART_Init(MYUBRR);
    
    while(1) {
        // Wait for a character from the Mac, then echo it back
        unsigned char received_char = USART_Receive();
        USART_Transmit(received_char);
    }
    return 0;
}