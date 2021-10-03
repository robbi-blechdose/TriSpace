#ifndef _SHIP_H
#define _SHIP_H

#include <SDL.h>
#include "engine/includes/3dMath.h"
#include "engine/util.h"
#include "cargo.h"

typedef struct {
    uint8_t maxShields;
    uint8_t maxEnergy;
    float shieldRegen;
    float energyRegen;
    float maxSpeed;
    float maxTurnSpeed;
} ShipType;

typedef struct {
    uint8_t damage;
    int16_t cooldown;
    uint8_t energyUsage;
} WeaponType;

typedef struct {
    uint8_t type;
    int16_t timer;
    float distanceToHit;
} Weapon;

typedef struct {
    uint8_t type;
    CargoHold hold;
    Weapon weapon;

    float shields;
    float energy;
    uint8_t fuel;

    vec3 position;
    vec3 rotation;
    float speed;
    float turnSpeedX;
    float turnSpeedY;

    uint8_t aiState;
    float aiRotX;
    float aiRotY;
} Ship;

#define SHIP_SPHERE_RADIUS 1.5f

#define MAX_FUEL 70

#define TYPE_NULL 255
extern const ShipType shipTypes[];
extern const WeaponType weaponTypes[];

void initShip();
void drawShip(Ship* ship);
void calcShip(Ship* ship, uint8_t collided, uint32_t ticks);
void steerShip(Ship* ship, int8_t dirX, int8_t dirY, uint32_t ticks);
void accelerateShip(Ship* ship, int8_t dir, uint32_t ticks);
uint8_t shipIsDestroyed(Ship* ship);
void fireWeapons(Ship* ship, Ship* targetShips, uint8_t numTargets);

#endif