#ifndef _SPACE_H
#define _SPACE_H

#include "../ship.h"
#include "starsystem.h"
#include "../player.h"
#include "../npcs/npc.h"

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

#define MAX_NPCS 9
#define NUM_NORM_NPCS 8
#define NPC_CONTRACT 8

#define SPAWN_INTERVAL_DISTANCE 80
#define SPAWN_CHANCE 15

void initUniverse(uint8_t* currentSystem, StarSystem* starSystem);

void initSystem(uint8_t* currentSystem, StarSystem* starSystem, Npc npcs[]);
void switchSystem(uint8_t* currentSystem, uint8_t newSystem[2], StarSystem* starSystem, Npc npcs[]);

void setInitialSpawnPos(vec3 playerPos);

void calcUniverse(State* state, StarSystem* starSystem, Player* player, Npc npcs[], uint32_t ticks);
void drawUniverse(State* state, StarSystem* starSystem, Npc npcs[]);

uint32_t getSeedForSystem(uint8_t x, uint8_t y);
float getDistanceToSystem(uint8_t currentSystem[2], uint8_t targetSystem[2]);

#endif