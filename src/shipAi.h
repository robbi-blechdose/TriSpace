#ifndef _SHIPAI_H
#define _SHIPAI_H

#include "ship.h"
#include "engine/util.h"

#define AI_RANGE 60

void calcNPCAi(Ship* playerShip, Ship* npcShip, uint32_t ticks);

#endif