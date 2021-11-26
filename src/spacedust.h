#include "ship.h"

#define NUM_SPACEDUST 16
#define SPACEDUST_COLOR 0xF5EDDF

void initSpacedust();
void calcSpacedust(Ship* playerShip, uint32_t ticks);
void drawSpacedust();