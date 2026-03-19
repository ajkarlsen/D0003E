#include "Reciever.h"
#include <avr/io.h>
#include "BridgeController.h"


void Receive_Data(Reciever *self, int arg) {
    unsigned char incoming_byte = UDR0;
    ASYNC(self->bridge, Data_Handler, incoming_byte); 
}

