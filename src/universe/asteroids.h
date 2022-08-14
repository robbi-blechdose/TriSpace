#ifndef _ASTEROIDS_H
#define _ASTEROIDS_H

#include <stdbool.h>

#include "../engine/util.h"

typedef struct {
    vec3 position;
    float size;
    float health;
} Asteroid;

#define NUM_ASTEROIDS 16
#define ASTEROID_FIELD_SIZE 40
#define ASTEROID_HEALTH 3

void initAsteroids();

void createAsteroids(vec3 pos);
void drawAsteroids();

bool checkAsteroidHit(vec3* position, vec3* direction, float damage, uint8_t mineChance);

Asteroid* getAsteroids();

#endif