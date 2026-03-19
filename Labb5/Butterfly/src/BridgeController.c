// BridgeController.c
#include <avr/io.h>
#include "BridgeController.h"
#include "Display.h"

// Define combined light states
#define LIGHTS_ALL_RED (LIGHT_N_RED | LIGHT_S_RED)


void Update_LCD(BridgeController *self) {
    if (self->display != NULL) {
        ASYNC((Display*)self->display, Update_LCD_Event, (int)self);
    }
}

void Start_Bridge(BridgeController *self, int arg) {
    // Send an initial All-Red command to sync the simulator
    Serial_Write(LIGHTS_ALL_RED);
    self->current_direction = DIR_EMPTY;
    Update_LCD(self);
}

// Runs when a car has spent 5 seconds on the bridge
void Car_Left_Bridge(BridgeController *self, int arg) {
    self->cars_on_bridge--;
    ASYNC(self, Update_Logic, 0); 
    Update_LCD(self);
}

// Core coordinator
void Update_Logic(BridgeController *self, int arg) {
    unsigned char next_light_state = LIGHTS_ALL_RED;
    if (self->cars_on_bridge == 0){
        if (self->current_direction == DIR_EMPTY) {
            if (self->north_queue > 0){
                self->current_direction = DIR_NORTH;
                self->passing_cars = 0;
            }
            else if (self->south_queue > 0){
                self->current_direction = DIR_SOUTH;
                self->passing_cars = 0;
            }
        }
        else if(self->current_direction == DIR_NORTH) {
            if (self->south_queue > 0){
                self->current_direction = DIR_SOUTH;
                self->passing_cars = 0;
            } else if (self->north_queue == 0) {
                self->current_direction = DIR_EMPTY;
                self->passing_cars = 0;
            }
        }
        else if(self->current_direction == DIR_SOUTH) {
            if (self->north_queue > 0){
                self->current_direction = DIR_NORTH;
                self->passing_cars = 0;
            } else if (self->south_queue == 0) {
                self->current_direction = DIR_EMPTY;
                self->passing_cars = 0;
            }
        }
    }

    if (self->cooldown == 1) {
        next_light_state = LIGHTS_ALL_RED;
    }
    else {
        if(self->current_direction == DIR_NORTH) {
            if(self->passing_cars >= 5 && self->south_queue > 0) {
                next_light_state = LIGHTS_ALL_RED; 
            } else if (self->north_queue > 0) {
                next_light_state = LIGHT_N_GREEN | LIGHT_S_RED;
            }
        }
        else if(self->current_direction == DIR_SOUTH) {
            if(self->passing_cars >= 5 && self->north_queue > 0) {
                next_light_state = LIGHTS_ALL_RED; 
            } else if (self->south_queue > 0) {
                next_light_state = LIGHT_N_RED | LIGHT_S_GREEN;
            }
        }
    }
    Serial_Write(next_light_state);
}
// Interrupt handler for USART

void End_Cooldown(BridgeController *self, int arg) {
    self->cooldown = 0;
    ASYNC(self, Update_Logic, 0); 
}

void Data_Handler(BridgeController *self, int incoming_byte) {
    if (incoming_byte & SENSOR_N_ARRIVE) {
        self->north_queue++;
        ASYNC(self, Update_Logic, 0);
    } 
    if (incoming_byte & SENSOR_S_ARRIVE) {
        self->south_queue++;
        ASYNC(self, Update_Logic, 0);
    }
    
    if (incoming_byte & SENSOR_N_ENTER) {
        self->north_queue--;
        self->cars_on_bridge++;
        self->passing_cars++;
        self->cooldown = 1; // Start cooldown to prevent immediate light change
        AFTER(SEC(1), self, End_Cooldown, 0); // Cooldown lasts 1s
        AFTER(SEC(5), self, Car_Left_Bridge, 0); // Leave in 5s
        ASYNC(self, Update_Logic, 0); // Check if we need to turn light red (e.g. for pacing)
    }
    
    if (incoming_byte & SENSOR_S_ENTER) {
        self->south_queue--;
        self->cars_on_bridge++;
        self->passing_cars++;
        self->cooldown = 1; // Start cooldown to prevent immediate light change
        AFTER(SEC(1), self, End_Cooldown, 0); // Cooldown lasts 1s
        AFTER(SEC(5), self, Car_Left_Bridge, 0); // Leave in 5s
        ASYNC(self, Update_Logic, 0); 
    }

    Update_LCD(self);
}   

