#ifndef _Reciever_
#define _Reciever_
#include "TinyTimber.h"
#include "BridgeController.h"

typedef struct {
    Object super;
    BridgeController *bridge; 
} Reciever;

#define initReciever(bridge_ptr) {initObject(), bridge_ptr};

void Receive_Data(Reciever *self, int arg);

#endif