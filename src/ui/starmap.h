#ifndef _STARMAP_H
#define _STARMAP_H

#include <stdint.h>
#include <stdbool.h>
#include "GL/gl.h"

void initStarmap(GLuint uiTex);
void quitStarmap();

void calcStarmap(uint32_t ticks);
void drawStarmap3d(uint8_t* currentSystem, float fuel, uint8_t contractSystem[2], bool contractActive);
void drawSystemInfoBox();

void moveStarmapCursor(int8_t dirX, int8_t dirY);

/**
 * Only used for loading to move the cursor to the newly loaded current system
 */
void setStarmapCursor(int8_t x, int8_t y);

int8_t* getStarmapCursor();

#endif