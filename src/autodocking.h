#ifndef AUTODOCKING_H
#define AUTODOCKING_H

#include <stdbool.h>
#include <stdint.h>

#include "engine/util.h"
#include "ship.h"
#include "universe/starsystem.h"

/**
 * Docking sequence looks like this:
 * 
 * target
 * O --- O approach
 *        \
 *         O turn
 *        /
 *       O start
 **/

#define NUM_POINTS 3
#define POINT_TURN     0
#define POINT_APPROACH 1
#define POINT_TARGET   2

typedef struct {
    uint8_t active;
    uint8_t pointIndex;
    vec3 points[NUM_POINTS];
} AutodockData;

#define DOCK_AUTO_DISTANCE     80
#define DOCK_APPROACH_DISTANCE  4

bool isAutodockPossible(Ship* ship, StarSystem* starSystem);
void preCalcAutodockShip(AutodockData* ad, Ship* ship, StarSystem* starSystem);
void calcAutodockShip(AutodockData* ad, Ship* ship, uint32_t ticks);

#endif