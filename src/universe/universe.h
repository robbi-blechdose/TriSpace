#ifndef _SPACE_H
#define _SPACE_H

#include "../ship.h"

typedef enum {
    NONE,
    //World states
    SPACE,
    STATION,
    //PLANET,
    HYPERSPACE,
    //GUI states
    SAVELOAD,
    TRADING,
    EQUIP,
    MAP,
    //Special states
    TITLE,
    GAME_OVER
} State;

#define MAX_NPC_SHIPS 8

void initUniverse(StarSystem* starSystem);
void calcUniverse(State* state, State* targetState, StarSystem* starSystem, Ship* playerShip, Ship npcShips[], uint32_t ticks);
void drawUniverse(State* state, StarSystem* starSystem, Ship npcShips[]);
void switchSystem(uint16_t newSystem, StarSystem* starSystem, Ship npcShips[], ShipType* type, WeaponType* weaponType);
uint32_t* getSystemSeeds();
uint32_t getCurrentSystem();

#endif