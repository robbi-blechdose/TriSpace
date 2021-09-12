#ifndef _SHIPAI_H
#define _SHIPAI_H

#include "ship.h"
#include "engine/util.h"

#define AI_RANGE 50
#define AI_RANGE_COMBAT_NEAR 15
#define AI_RANGE_COMBAT_FAR 30

void calcNPCAi(Ship* playerShip, Ship* npcShip, uint32_t ticks);

#endif