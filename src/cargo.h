#ifndef _CARGO_H
#define _CARGO_H

#include <SDL.h>

#define NUM_CARGO_TYPES 4

typedef enum {
    Food,
    Textiles,
    Computers,
    Machinery
} CargoType;

typedef struct {
    uint8_t size;
    uint8_t cargo[NUM_CARGO_TYPES];
} CargoHold;

void getPriceForCargo(CargoType type, uint8_t techLevel);

#endif