#ifndef _SHIPAI_H
#define _SHIPAI_H

#include "ship.h"
#include "engine/util.h"

#define AI_RANGE 50
#define AI_RANGE_CIRCLE_INNER 25
#define AI_RANGE_CIRCLE_OUTER 26
#define AI_RANGE_TOONEAR 15
#define AI_RANGE_TOONEAR2 10

#define STATE_IDLE 0
#define STATE_CIRCLE 1
#define STATE_ATTACK 2
#define STATE_FLEE 3

//-20 will never occur (since we wrap values to be in the interval [0, 2 * M_PI]), so we use it as "uninitialized"
#define AI_ROT_NONE -20.0f

void calcNPCAi(Ship* playerShip, Ship* npcShip, uint32_t ticks);

#endif