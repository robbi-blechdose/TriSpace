#ifndef _UI_H
#define _UI_H

#include "ship.h"
#include "universe/universe.h"

void initUI();
void drawUI(State state, Ship* playerShip, Ship npcShips[], vec3 stationPos);
void drawTradingUI(CargoHold* playerHold, CargoHold* stationHold, SystemInfo* info);
void moveTradeCursor(int8_t dir);
uint8_t getTradeCursor();

#endif