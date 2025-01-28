#ifndef STARSYSTEM_H
#define STARSYSTEM_H

#include <stdbool.h>
#include <stdint.h>

#include "GL/gl.h"
#include "../fk-engine-core/util.h"

#define MAX_STARS 3
#define MAX_PLANETS 6
#define MAX_TECH_LEVEL 15
#define MAX_GOVERNMENT 5
#define MAX_NAME_LENGTH 28

//Planet types are ordered by closeness to the sun
typedef enum {
    PT_Venus,
    PT_Mars,
    PT_Earth,
    PT_Ocean,
    PT_Forest,
    PT_Ice,
    PT_Dead,
    PT_Gas,
    PT_DarkCities,
    NUM_PLANET_TYPES
} PlanetType;

typedef enum {
    ST_NORMAL,
    ST_BLUE,
    ST_RED,
    NUM_STAR_TYPES
} StarType;

typedef struct {
    uint8_t techLevel;
    uint8_t government;
    //These diff values must be in the range [-2, 2]!
    int8_t treeDiff;
    int8_t rockDiff;
    int8_t waterDiff;
} SystemCharacteristics;

typedef struct {
    char name[MAX_NAME_LENGTH];

    uint8_t numStars;
    StarType starTypes[MAX_STARS];

    uint8_t numPlanets;
    PlanetType planetTypes[MAX_PLANETS];

    uint8_t stationPlanetIndex;
    bool hasAsteroidField;

    SystemCharacteristics characteristics;
} StarSystemInfo;

typedef struct {
    float size;
    vec3 position;
} Star;

typedef struct {
    float size;
    vec3 position;
    GLuint texture;
    bool hasRing;
} Planet;

typedef struct {
    vec3 position;
    vec3 dockingPosition;
    vec3 exitPosition;
} SpaceStation;

typedef struct {
    StarSystemInfo info;
    Star stars[MAX_STARS];
    Planet planets[MAX_PLANETS];
    SpaceStation station;
    vec3 asteroidFieldPos;
} StarSystem;

void initStarSystem();
void quitStarSystem();

void deleteStarSystem(StarSystem* system);
void drawStarSystem(StarSystem* system);

bool hasDockingDistance(vec3* pos, vec3* dockingPos);

/**
 * Returns a random position in a star system that's free of objects - stars, planets, ...
 * @param center The center point from which random positions are calculated
 * @param bounds The bounds in which the random position can be
 * @param minDistanceFromObjects The minimum distance from an object the position must have
 * @param minDistanceFromCenter The minimum distance from the center the position must have
 **/
vec3 getRandomFreePosBounds(StarSystem* system, vec3 center, vec3 bounds, float minDistanceFromObjects, float minDistanceFromCenter);
/**
 * Returns a random position in a star system that's free of objects - stars, planets, ...
 * @param minDistanceFromObjects The minimum distance from an object the position must have
 **/
vec3 getRandomFreePos(StarSystem* system, float minDistanceFromObjects);

bool hasSunFuelDistance(StarSystem* system, vec3* pos);

#endif