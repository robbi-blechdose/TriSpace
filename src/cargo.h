#ifndef _CARGO_H
#define _CARGO_H

#include <stdbool.h>
#include <stdint.h>

#include "universe/starsystem.h"

#define NUM_CARGO_TYPES 14

typedef enum {
    Food,
    Textiles,
    Liquor,
    Furs,
    Radioactives,
    Luxuries,

    //Technical goods
    Computers,
    Machinery,

    //Mine-able goods
    Gold,
    Platinum,
    Dilithium,

    //Illegal goods
    Slaves,
    Firearms,
    Narcotics
} CargoType;

#define CARGO_HOLD_SIZE_NORM  25
#define CARGO_HOLD_SIZE_LARGE 30

typedef struct {
    uint16_t size;
    uint8_t cargo[NUM_CARGO_TYPES];
    uint16_t money;
} CargoHold;

uint8_t getPriceForCargo(CargoType type, SystemInfo* info);
void printNameForCargo(char* str, CargoType type);
void printUnitForCargo(char* str, CargoType type);

bool isCargoIllegal(CargoType type);

/**
 * Calculates the current size of a cargo hold (the amount of cargo units it contains)
 **/
uint8_t getCargoHoldSize(CargoHold* hold);

/**
 * Transfers cargo between two holds, using the current star system for price calculations
 **/
bool transferCargo(CargoHold* holdSell, CargoHold* holdBuy, CargoType type, SystemInfo* info, uint8_t limit);

void createStationHold(CargoHold* hold);

#endif