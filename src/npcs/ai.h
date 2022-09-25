#ifndef _NPC_AI
#define _NPC_AI

#include "npc.h"
#include "../player.h"

#define AI_RANGE               75
#define AI_RANGE_WAYPOINT       2

#define AI_RANGE_CIRCLE        25
#define AI_RANGE_VEEROFF        8
#define AI_RANGE_FLEE          15

#define AI_RANGE_POLICECHECK  150

void calcNPCAi(Npc* npc, Player* player, Npc* npcs, uint32_t ticks);

#endif