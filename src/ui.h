#ifndef _UI_H
#define _UI_H

#include <SDL.h>
#include "ship.h"
#include "universe/universe.h"

void initUI();
void drawUI(State state, Ship* playerShip, Ship npcShips[]);

#endif