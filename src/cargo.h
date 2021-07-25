#ifndef _CARGO_H
#define _CARGO_H

#include <stdint.h>
#include "universe/starsystem.h"

#define NUM_CARGO_TYPES 4

typedef enum {
    Food,
    Textiles,
    Computers,
    Machinery
} CargoType;

typedef struct {
    uint16_t size;
    uint8_t cargo[NUM_CARGO_TYPES];
    uint16_t money;
} CargoHold;

uint8_t getPriceForCargo(CargoType type, SystemInfo* info);
void printNameForCargo(char* str, CargoType type);
void printUnitForCargo(char* str, CargoType type);
uint8_t transferCargo(CargoHold* holdSell, CargoHold* holdBuy, CargoType type, SystemInfo* info);
void createStationHold(CargoHold* hold);

#endif