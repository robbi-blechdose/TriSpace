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
    float hitSphere;
} ShipType;

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

    //AI-specific stuff
    uint8_t aiState;
    float aiRotX;
    float aiRotY;
    uint8_t damaged;
} Ship;

#define MAX_FUEL 70

#define NUM_SHIP_TYPES 3
#define SHIP_TYPE_SMALLPIRATE 0
#define SHIP_TYPE_CRUISELINER 1
#define SHIP_TYPE_POLICE      2
#define SHIP_TYPE_NULL 255
extern const ShipType shipTypes[];
extern const WeaponType weaponTypes[];

#define DAMAGE_SOURCE_PLAYER 1
#define DAMAGE_SOURCE_NPC    2

void initShip();

void drawShip(Ship* ship);
void calcShip(Ship* ship, uint8_t collided, uint32_t ticks);
void steerShip(Ship* ship, int8_t dirX, int8_t dirY, uint32_t ticks);
void accelerateShip(Ship* ship, int8_t dir, uint32_t ticks);
uint8_t shipIsDestroyed(Ship* ship);

/**
 * Fires the weapons of a ship in the forward direction
 * Checks for hits against target ships and asteroids
 * @param ship The ship which fires
 * @param targetShips Pointer to an array of ships (or one) which can be damaged
 * @param numTargets The length of the targetShips array
 * @param source A DAMAGE_SOURCE value to determine which type of ship fired
 **/
void fireWeapons(Ship* ship, Ship* targetShips, uint8_t numTargets, uint8_t source);

float getTurnSpeedForRotation(float current, float target, float maxSpeed);

#endif