#ifndef _STARMAP_H
#define _STARMAP_H

#include <stdint.h>

void initStarmap();
void calcStarmap(uint32_t ticks);
void drawStarmap3d(uint8_t* currentSystem, float fuel);
void drawSystemInfoBox();

void moveStarmapCursor(int8_t dirX, int8_t dirY);
int8_t* getStarmapCursor();

#endif