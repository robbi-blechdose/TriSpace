#ifndef _SPACE_H
#define _SPACE_H

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

void initUniverse(StarSystem* starSystem);

void initSystem(uint8_t* currentSystem, StarSystem* starSystem, Npc npcs[]);
void switchSystem(uint8_t* currentSystem, uint8_t newSystem[2], StarSystem* starSystem, Npc npcs[]);

void calcNPCShips(StarSystem* starSystem, Player* player, Npc npcs[], uint32_t ticks);

void setInitialSpawnPos(vec3 playerPos);
void calcUniverseSpawnNPCShips(StarSystem* starSystem, Ship* playerShip, Npc npcs[], uint32_t ticks);

uint32_t getSeedForSystem(uint8_t x, uint8_t y);
float getDistanceToSystem(uint8_t currentSystem[2], uint8_t targetSystem[2]);

#endif