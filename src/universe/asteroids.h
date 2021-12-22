#ifndef _ASTEROIDS_H
#define _ASTEROIDS_H

#include "../engine/util.h"

typedef struct {
    vec3 position;
    float size;
} Asteroid;

#define NUM_ASTEROIDS 16

void initAsteroids();
void drawAsteroids();

void damageAsteroid(uint8_t index);

#endif