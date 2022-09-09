#ifndef _STARMAP_H
#define _STARMAP_H

#include <stdint.h>

void initStarmap();
void calcStarmap(uint8_t currentSystem[2], float fuel);
void drawStarmap();

#endif