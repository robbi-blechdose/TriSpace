#ifndef UNIVERSE_H
#define UNIVERSE_H

#include "../ship.h"
#include "starsystem.h"
#include "../player.h"
#include "../npcs/npc.h"

#define BASE_SEED 1
#define UNIVERSE_SIZE 16

#define MAX_NPCS 9
#define NUM_NORM_NPCS 8
#define NPC_CONTRACT 8

#define SPAWN_INTERVAL_DISTANCE 80
#define SPAWN_CHANCE 15

void initUniverse();

void initSystem(int8_t* currentSystem, StarSystem* system, Npc npcs[]);
void switchSystem(int8_t* currentSystem, int8_t newSystem[2], StarSystem* system, Npc npcs[]);

void calcNPCShips(StarSystem* system, Player* player, Npc npcs[], uint32_t ticks);

void setInitialSpawnPos(vec3 playerPos);
void calcUniverseSpawnNPCShips(StarSystem* system, Ship* playerShip, Npc npcs[], uint32_t ticks);

uint32_t getSeedForSystem(int8_t x, int8_t y);
float getDistanceToSystem(int8_t currentSystem[2], int8_t targetSystem[2]);

#endif