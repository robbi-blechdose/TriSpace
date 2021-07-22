#ifndef _STARSYSTEM_H
#define _STARSYSTEM_H

#include <SDL.h>
#include "GL/gl.h"
#include "../engine/util.h"
#include "../ship.h"

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
    Star stars[3];
    uint8_t numStars;
    Planet planets[8];
    uint8_t numPlanets;
    SpaceStation station;
} StarSystem;

void initStarSystem();
void deleteStarSystem();
void drawStarSystem();

StarSystem* getStarSystem();

uint8_t hasDockingDistance(Ship* ship);
vec3 getExitPosition();

#endif