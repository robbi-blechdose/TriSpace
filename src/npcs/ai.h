#ifndef _NPC_AI
#define _NPC_AI

#include "npc.h"
#include "../player.h"

#define AI_RANGE               75
#define AI_RANGE_CIRCLE_INNER  25
#define AI_RANGE_CIRCLE_OUTER  26
#define AI_RANGE_TOONEAR       15
#define AI_RANGE_POLICECHECK  150

void calcNPCAi(Npc* npc, Player* player, Npc* npcs, uint32_t ticks);

#endif