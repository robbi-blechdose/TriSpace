#ifndef _SPACE_H
#define _SPACE_H

#include "../ship.h"

typedef enum {
    NONE,
    SPACE,
    STATION,
    PLANET
} State;

#define MAX_NPC_SHIPS 8

void initUniverse();
void calcUniverse(State* state, State* targetState, Ship* playerShip, Ship npcShips[]);
void drawUniverse(State* state, Ship npcShips[]);

#endif