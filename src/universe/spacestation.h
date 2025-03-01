#ifndef SPACESTATION_H
#define SPACESTATION_H

#include <stdbool.h>

#include "../fk-engine-core/includes/3dMath.h"

void initSpaceStation();
void quitSpaceStation();

void drawSpaceStation();

bool hasLeavingDistance(vec3 pos);
bool hasLandingDistance(vec3 pos);

#endif