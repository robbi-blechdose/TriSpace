#ifndef _ASTEROIDS_H
#define _ASTEROIDS_H

#include "../engine/util.h"

typedef struct {
    vec3 position;
    float size;
} Asteroid;

#define NUM_ASTEROIDS 16
#define ASTEROID_FIELD_SIZE 35

void initAsteroids();

void createAsteroids(vec3 pos);
void drawAsteroids();

void damageAsteroid(uint8_t index);

#endif