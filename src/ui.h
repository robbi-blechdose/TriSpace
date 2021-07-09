#ifndef _UI_H
#define _UI_H

#include <SDL.h>
#include "ship.h"

void initUI();
void drawUI(SDL_Surface* screen, Ship* playerShip);

#endif