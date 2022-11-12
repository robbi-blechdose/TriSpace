#ifndef _MISSILES_H
#define _MISSILES_H

#include <stdint.h>
#include "../engine/includes/3dMath.h"
#include "../engine/quaternion.h"
#include "../ship.h"

void initMissiles();
void quitMissiles();

void createMissile(vec3 pos, quat rot, Ship* targetShip);
void calcMissiles(uint32_t ticks);
void drawMissiles();

#endif