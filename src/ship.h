#ifndef _SHIP_H
#define _SHIP_H

#include <SDL.h>
#include "engine/util.h"
#include "cargo.h"

typedef struct {
    uint8_t maxShields;
    uint8_t maxEnergy;
    float maxSpeed;
    float maxTurnSpeed;
} ShipType;

typedef struct {
    ShipType* type;

    Vector3 position;
    Vector3 rotation;
    float speed;
    float turnSpeedX;
    float turnSpeedY;

    uint8_t shields;
    uint8_t energy;

    CargoHold hold;
} Ship;

void initShip();
void drawShip(Ship* ship);
void calcShip(Ship* ship, uint32_t ticks);
void steerShip(Ship* ship, int8_t dirX, int8_t dirY, uint32_t ticks);
void accelerateShip(Ship* ship, int8_t dir, uint32_t ticks);
void fireWeapons(Ship* ship);

#endif