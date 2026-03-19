// BridgeController.h
#ifndef _BridgeController_
#define _BridgeController_

#include "TinyTimber.h"
#include "Display.h"

// Sensor Input Bits (from Simulator)
#define SENSOR_N_ARRIVE 1  // Bit 0
#define SENSOR_N_ENTER  2  // Bit 1
#define SENSOR_S_ARRIVE 4  // Bit 2
#define SENSOR_S_ENTER  8  // Bit 3

// Trasmiter Output Bits (to Simulator)
#define LIGHT_N_GREEN   1  // Bit 0
#define LIGHT_N_RED     2  // Bit 1
#define LIGHT_S_GREEN   4  // Bit 2
#define LIGHT_S_RED     8  // Bit 3

// Direction states
#define DIR_EMPTY 0
#define DIR_NORTH 1
#define DIR_SOUTH 2

typedef struct {
    Object super;
    int north_queue;
    int south_queue;
    int cars_on_bridge;
    int current_direction;
    int passing_cars; // Tracks cars that have entered but not left
    Display *display; // Reference to the Display object for LCD updates
    int cooldown;
} BridgeController;

// Macro to initialize the object at startup
#define initBridgeController(display_ptr) { initObject(), 0, 0, 0, DIR_EMPTY, 0, display_ptr, 0 }
void Init_Hardware(void);
void Serial_Write(unsigned char data);
void Start_Bridge(BridgeController *self, int arg);
void Car_Left_Bridge(BridgeController *self, int arg);
void Update_Logic(BridgeController *self, int arg);
void Update_LCD(BridgeController *self);
void Data_Handler(BridgeController *self, int arg);
void End_Cooldown(BridgeController *self, int arg);

#endif