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
    CONTRACTS,
    MAP,
    //Special states
    TITLE,
    GAME_OVER
} State;

#define BASE_SEED 1
#define UNIVERSE_SIZE 16

#define MAX_NPC_SHIPS 8

void initUniverse(uint8_t* currentSystem, StarSystem* starSystem);
void calcUniverse(State* state, StarSystem* starSystem, Ship* playerShip, Ship npcShips[], uint32_t ticks);
void drawUniverse(State* state, StarSystem* starSystem, Ship npcShips[]);
void switchSystem(uint8_t* currentSystem, uint8_t newSystem[2], StarSystem* starSystem, Ship npcShips[]);
uint32_t getSeedForSystem(uint8_t x, uint8_t y);
float getDistanceToSystem(uint8_t currentSystem[2], uint8_t targetSystem[2]);

#endif