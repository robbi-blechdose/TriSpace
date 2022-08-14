#ifndef _SPACESTATION_H
#define _SPACESTATION_H

#include <stdint.h>
#include <stdbool.h>

#include "../engine/includes/3dMath.h"

void initSpaceStation();
void drawSpaceStation();

bool hasLeavingDistance(vec3 pos);
bool hasLandingDistance(vec3 pos);

#endif