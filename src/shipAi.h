#ifndef _SHIPAI_H
#define _SHIPAI_H

#include "engine/util.h"

#include "ship.h"
#include "player.h"

#define AI_RANGE               75
#define AI_RANGE_CIRCLE_INNER  25
#define AI_RANGE_CIRCLE_OUTER  26
#define AI_RANGE_TOONEAR       15
#define AI_RANGE_POLICECHECK  150

#define STATE_IDLE         0
#define STATE_CIRCLE       1
#define STATE_ATTACK       2
#define STATE_FLEE         3
#define STATE_POLICE_CHECK 4

//-20 will never occur (since we wrap values to be in the interval [0, 2 * M_PI]), so we use it as "uninitialized"
#define AI_ROT_NONE -20.0f

#define AI_FIRING_CHANCE 75

void calcNPCAi(Player* player, Ship* npcShip, Ship* npcShips, uint32_t ticks);

#endif