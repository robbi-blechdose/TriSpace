#ifndef _SPACESTATION_H
#define _SPACESTATION_H

#include <stdint.h>
#include "../engine/includes/3dMath.h"

void initSpaceStation();
void drawSpaceStation();

uint8_t hasLeavingDistance(vec3 pos);
uint8_t hasLandingDistance(vec3 pos);

#endif