#ifndef SHIP_COLLISIONS_H
#define SHIP_COLLISIONS_H

#include "ship.h"
#include "universe/starsystem.h"

#include <stdbool.h>

bool checkStarSystemCollision(Ship* ship, StarSystem* starSystem);
bool checkStationCollision(Ship* ship);

#endif