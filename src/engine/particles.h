#ifndef _PARTICLES_H
#define _PARTICLES_H

#include <SDL.h>
#include "util.h"

void createParticles();
void calcParticles(uint32_t steps);
void drawParticles();

#endif