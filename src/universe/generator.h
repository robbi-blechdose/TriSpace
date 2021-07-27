#ifndef _GENERATOR_H
#define _GENERATOR_H

#include <SDL.h>
#include "starsystem.h"

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

uint8_t getNumStarsForSystem(uint32_t seed);
void generateStarSystem(StarSystem* system, uint32_t seed);
void generateSystemSeeds(uint32_t* systemSeeds, uint32_t baseSeed);


#endif