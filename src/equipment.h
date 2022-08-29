#ifndef _EQUIPMENT_H
#define _EQUIPMENT_H

#include <stdbool.h>

#include "ship.h"

#define NUM_EQUIPMENT_TYPES 7

typedef enum {
    Fuel,
    Cargo30,
    LaserMkII,
    LaserMkIII,
    MiningLaser,
    DockingComputer,
    FuelScoops
} EquipmentType;

uint16_t getPriceForEquipment(EquipmentType type);
void printNameForEquipment(char* str, EquipmentType type);
void printEquipmentStatusForShip(char* str, Ship* ship, EquipmentType type);
bool buyEquipment(Ship* ship, EquipmentType type);

#endif