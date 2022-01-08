#ifndef _EQUIPMENT_H
#define _EQUIPMENT_H

#include "ship.h"

#define NUM_EQUIPMENT_TYPES 6

typedef enum {
    Fuel,
    Cargo30,
    LaserMkII,
    LaserMkIII,
    MiningLaser,
    DockingComputer
} EquipmentType;

uint16_t getPriceForEquipment(EquipmentType type);
void printNameForEquipment(char* str, EquipmentType type);
void printEquipmentStatusForShip(char* str, Ship* ship, EquipmentType type);
uint8_t buyEquipment(Ship* ship, EquipmentType type);

#endif