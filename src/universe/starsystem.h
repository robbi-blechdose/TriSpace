#ifndef _STARSYSTEM_H
#define _STARSYSTEM_H

#include <SDL.h>
#include "GL/gl.h"
#include "../engine/util.h"

#define MAX_PLANETS 6
#define MAX_TECH_LEVEL 15
#define MAX_GOVERNMENT 5
#define MAX_NAME_LENGTH 28

typedef struct {
    uint8_t techLevel;
    uint8_t government;
    //These diff values must be in the range [-2, 2]!
    int8_t treeDiff;
    int8_t rockDiff;
    int8_t waterDiff;
    char name[MAX_NAME_LENGTH];
} SystemInfo;

typedef struct {
    float size;
    vec3 position;
} Star;

typedef struct {
    float size;
    vec3 position;
    GLuint texture;
    uint8_t hasRing;
} Planet;

typedef struct {
    vec3 position;
    vec3 dockingPosition;
    vec3 exitPosition;
} SpaceStation;

typedef struct {
    SystemInfo info;
    Star stars[3];
    uint8_t numStars;
    Planet planets[MAX_PLANETS];
    uint8_t numPlanets;
    SpaceStation station;
    uint8_t hasAsteroidField;
    vec3 asteroidFieldPos;
} StarSystem;

void initStarSystem();
void deleteStarSystem(StarSystem* starSystem);
void drawStarSystem(StarSystem* starSystem);

uint8_t hasDockingDistance(vec3* pos, vec3* dockingPos);

/**
 * Returns a random position in a star system that's free of objects - stars, planets, ...
 * @param center The center point from which random positions are calculated
 * @param bounds The bounds in which the random position can be
 * @param minDistanceFromObjects The minimum distance from an object the position must have
 * @param minDistanceFromCenter The minimum distance from the center the position must have
 **/
vec3 getRandomFreePosBounds(StarSystem* starSystem, vec3 center, vec3 bounds, float minDistanceFromObjects, float minDistanceFromCenter);
/**
 * Returns a random position in a star system that's free of objects - stars, planets, ...
 * @param minDistanceFromObjects The minimum distance from an object the position must have
 **/
vec3 getRandomFreePos(StarSystem* starSystem, float minDistanceFromObjects);

#endif