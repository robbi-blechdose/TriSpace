#ifndef TITLE_SCREEN_H
#define TITLE_SCREEN_H

#include <stdint.h>
#include <stdbool.h>

void initTitleScreen();
void quitTitleScreen();

void calcTitleScreen(uint32_t ticks);

void drawTitleScreen3d();
void drawTitleScreen();

void scrollTitleScreenCursor(int8_t dir);
int8_t getTitleScreenCursor();

bool calcCredits(uint32_t ticks);
void drawCredits();

#endif