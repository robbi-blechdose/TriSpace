#ifndef _SPACE_H
#define _SPACE_H

#include "../ship.h"

typedef enum {
    NONE,
    //World states
    SPACE,
    STATION,
    //PLANET,
    //GUI states
    TRADING,
    //EQUIPPING,
    MAP
} State;

#define MAX_NPC_SHIPS 8

void initUniverse();
void calcUniverse(State* state, State* targetState, Ship* playerShip, Ship npcShips[]);
void drawUniverse(State* state, Ship npcShips[]);
void switchSystem(uint16_t newSystem);
uint32_t* getSystemSeeds();

#endif