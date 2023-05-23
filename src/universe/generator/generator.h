#ifndef _GENERATOR_H
#define _GENERATOR_H

#include <SDL.h>
#include "../starsystem.h"
#include "../../engine/util.h"

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
    NUM_PLANET_TYPES
} PlanetType;

typedef struct {
    uint8_t numStars;
    uint8_t numPlanets;
    uint8_t paletteIndices[8];
    uint8_t spIndex;
    SystemInfo info;
} SystemBaseData;

uint8_t getNumStarsForSystem(uint32_t seed);
void generateSystemBaseData(SystemBaseData* sbd, uint32_t seed);
void generateStarSystem(StarSystem* system, uint32_t seed);
vec2 generateSystemPos(uint32_t seed, uint8_t i, uint8_t j);
void generateSystemName(char* buffer);
void generateSystemDescription(char* buffer, SystemBaseData* sbd);

#endif