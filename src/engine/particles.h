#ifndef _PARTICLES_H
#define _PARTICLES_H

#include <SDL.h>
#include "util.h"

#define NUM_PARTICLES 64

typedef struct {
    vec3 positions[NUM_PARTICLES];
    vec3 directions[NUM_PARTICLES];
    int16_t lives[NUM_PARTICLES];
} Particles;

typedef enum {
    EXPLOSION
} EmitterType;

uint8_t createEmitter(vec3 position);
void calcParticles(uint32_t steps);
void drawParticles();

#endif