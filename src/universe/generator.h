#ifndef _GENERATOR_H
#define _GENERATOR_H

#include <SDL.h>
#include "starsystem.h"

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

void generateStarSystem(StarSystem* system, uint32_t seed);

#endif