#ifndef ASTEROIDS_H
#define ASTEROIDS_H

#include <stdbool.h>

#include "../engine/util.h"

#include "../player.h"

typedef struct {
    vec3 position;
    float size;
    float health;
} Asteroid;

#define NUM_ASTEROIDS 16
#define ASTEROID_FIELD_SIZE 40
#define ASTEROID_HEALTH 3

void initAsteroids();
void quitAsteroids();

void createAsteroids(vec3 pos);
void drawAsteroids();

Asteroid* getAsteroids();

void checkWeaponsAsteroidHit(Player* player);

#endif