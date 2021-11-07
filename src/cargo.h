#ifndef _CARGO_H
#define _CARGO_H

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

typedef struct {
    uint16_t size;
    uint8_t cargo[NUM_CARGO_TYPES];
    uint16_t money;
} CargoHold;

uint8_t getPriceForCargo(CargoType type, SystemInfo* info);
void printNameForCargo(char* str, CargoType type);
void printUnitForCargo(char* str, CargoType type);
uint8_t isCargoIllegal(CargoType type);
uint8_t getCargoHoldSize(CargoHold* hold);
uint8_t transferCargo(CargoHold* holdSell, CargoHold* holdBuy, CargoType type, SystemInfo* info);
void createStationHold(CargoHold* hold);

#endif