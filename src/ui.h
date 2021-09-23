#ifndef _UI_H
#define _UI_H

#include "ship.h"
#include "universe/universe.h"

#define RADAR_RANGE 100

//Macro for centering text
//X is the size of the text in characters
//X * 8 / 2 = X * 4
#define CENTER(X) (120 - (X) * 4)

void initUI();

void drawUI(State state, Ship* playerShip, Ship npcShips[], vec3 stationPos);

void drawTradingUI(CargoHold* playerHold, CargoHold* stationHold, SystemInfo* info);
void moveTradeCursor(int8_t dir);
uint8_t getTradeCursor();

void drawEquipUI(Ship* playerShip);

void drawMap(uint32_t systemSeeds[]);
void moveMapCursor(int8_t x, int8_t y);
uint16_t getMapCursor();

#endif