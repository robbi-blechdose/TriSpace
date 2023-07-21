#ifndef SPACEDUST_H
#define SPACEDUST_H

#include "ship.h"

#define NUM_SPACEDUST 16

void initSpacedust();
void calcSpacedust(Ship* playerShip, uint32_t ticks);
void drawSpacedust();

#endif