#ifndef _EQUIPMENT_H
#define _EQUIPMENT_H

#include <stdbool.h>

#include "player.h"

#define NUM_EQUIPMENT_TYPES 8

typedef enum {
    Fuel,
    Cargo30,
    LaserMkII,
    LaserMkIII,
    MiningLaser,
    DockingComputer,
    FuelScoops,
    Missile
} EquipmentType;

uint16_t getPriceForEquipment(EquipmentType type);
void printNameForEquipment(char* str, EquipmentType type);
void printEquipmentStatusForShip(char* str, Player* player, EquipmentType type);
bool buyEquipment(Player* player, EquipmentType type);

#endif