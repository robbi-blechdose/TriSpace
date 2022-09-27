#ifndef _SHIP_H
#define _SHIP_H

#include <stdbool.h>
#include <SDL.h>

#include "engine/includes/3dMath.h"
#include "engine/util.h"
#include "engine/quaternion.h"

typedef struct {
    uint8_t maxShields;
    uint8_t maxEnergy;
    float shieldRegen;
    float energyRegen;
    float maxSpeed;
    float maxTurnSpeed;
    float hitSphere;
} ShipTypeData;

typedef struct {
    uint8_t damage;
    int16_t cooldown;
    uint8_t energyUsage;
    uint8_t mineChance;
} WeaponType;

typedef struct {
    uint8_t type;
    int16_t timer;
    float distanceToHit;
} Weapon;

#define NUM_SHIP_TYPES 4
typedef enum {
    SHIP_TYPE_SMALLPIRATE,
    SHIP_TYPE_SMALLPIRATE2,
    SHIP_TYPE_CRUISELINER,
    SHIP_TYPE_POLICE,
    SHIP_TYPE_NULL = 255
} ShipType;

typedef struct {
    ShipType type;
    Weapon weapon;

    float shields;
    float energy;

    vec3 position;
    quat rotation;
    float speed;
    float turnSpeedX;
    float turnSpeedY;

    uint8_t damaged;
} Ship;

#define MAX_FUEL 15

extern const ShipTypeData shipTypes[];
extern const WeaponType weaponTypes[];

#define DAMAGE_SOURCE_PLAYER 1
#define DAMAGE_SOURCE_NPC    2

void initShip();

void drawShip(Ship* ship);
void calcShip(Ship* ship, uint8_t collided, uint32_t ticks);
void steerShip(Ship* ship, int8_t dirX, int8_t dirY, uint32_t ticks);
void accelerateShipLimit(Ship* ship, int8_t dir, uint32_t ticks, float max);
void accelerateShip(Ship* ship, int8_t dir, uint32_t ticks);
bool shipIsDestroyed(Ship* ship);

bool fireWeapons(Ship* ship);
bool checkWeaponsShipHit(Ship* ship, Ship* targetShips[], uint8_t numTargets, uint8_t source);

float getTurnSpeedForRotation(float current, float target, float maxSpeed);

#endif