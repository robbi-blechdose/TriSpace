#ifndef MISSILES_H
#define MISSILES_H

#include <stdint.h>
#include "../fk-engine-core/includes/3dMath.h"
#include "../fk-engine-core/quaternion.h"
#include "../ship.h"

void initMissiles();
void quitMissiles();

void createMissile(vec3 pos, quat rot, Ship* targetShip);
void calcMissiles(uint32_t ticks);
void drawMissiles();

#endif