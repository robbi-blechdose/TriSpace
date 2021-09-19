#ifndef _STARSYSTEM_H
#define _STARSYSTEM_H

#include <SDL.h>
#include "GL/gl.h"
#include "../engine/util.h"

#define MAX_PLANETS 6
#define MAX_TECH_LEVEL 15
#define MAX_GOVERNMENT 5

typedef struct {
    uint8_t techLevel;
    uint8_t government;
    //These diff values must be in the range [-2, 2]!
    int8_t treeDiff;
    int8_t rockDiff;
    int8_t waterDiff;
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
} StarSystem;

void initStarSystem();
void deleteStarSystem(StarSystem* starSystem);
void drawStarSystem(StarSystem* starSystem);

uint8_t hasDockingDistance(vec3* pos, vec3* dockingPos);

#endif