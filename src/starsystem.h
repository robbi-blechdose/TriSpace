#ifndef _STARSYSTEM_H
#define _STARSYSTEM_H

#include <SDL.h>
#include "engine/util.h"

typedef struct {
    float size;
    Vector3 position;
} Star;

typedef struct {
    float size;
    Vector3 position;
} Planet;

void initStarSystem();
void loadStarSystem();
void drawStarSystem();

#endif