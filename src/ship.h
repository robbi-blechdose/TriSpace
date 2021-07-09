#ifndef _SHIP_H
#define _SHIP_H

#include <SDL.h>
#include "engine/util.h"

typedef struct {
    uint8_t maxShields;
    uint8_t maxEnergy;
    float maxSpeed;
} ShipType;

typedef struct {
    ShipType* type;
    Vector3 position;
    Vector3 rotation;
    float speed;
    uint8_t shields;
    uint8_t energy;
} Ship;

void calcShip(Ship* ship, uint32_t ticks);
void steerShip(Ship* ship, int8_t dirX, int8_t dirY, uint32_t ticks);
void accelerateShip(Ship* ship, int8_t dir, uint32_t ticks);

#endif