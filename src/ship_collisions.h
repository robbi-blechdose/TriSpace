#include "ship.h"
#include "universe/starsystem.h"

#include <stdbool.h>

bool checkStarSystemCollision(Ship* ship, StarSystem* starSystem);
bool checkStationCollision(Ship* ship);