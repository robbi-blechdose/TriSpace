#ifndef _TITLE_SCREEN_H
#define _TITLE_SCREEN_H

#include <stdint.h>

void initTitleScreen();

void calcTitleScreen(uint32_t ticks);

void drawTitleScreen3d();
void drawTitleScreen();

void toggleTitleScreenCursor();
int8_t getTitleScreenCursor();

#endif