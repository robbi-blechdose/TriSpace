#ifndef GENERATOR_H
#define GENERATOR_H

#include <stdint.h>
#include "../starsystem.h"
#include "../../engine/util.h"

/**
 * Generate basic information about a star system
 **/
void generateStarSystemInfo(StarSystemInfo* info, uint32_t seed);

/**
 * Generate a full star system, including:
 * - texture data for planets
 * - positions for all bodies
 **/
void generateStarSystem(StarSystem* system, uint32_t seed);

vec2 generateSystemPos(uint32_t seed, uint8_t i, uint8_t j);
void generateSystemName(char* buffer);
void generateSystemDescription(char* buffer, StarSystemInfo* info);

#endif