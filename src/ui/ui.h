#ifndef _UI_H
#define _UI_H

#include <stdbool.h>

#include "../ship.h"
#include "../universe/universe.h"
#include "../player.h"
#include "../npcs/npc.h"

#define RADAR_RANGE 120

void initUI();
void quitUI();

void drawUI(bool onStation, Player* player, Npc npcs[], vec3 stationPos, uint8_t autodockPossible);

void drawSaveLoadUI(uint8_t cursor);

void drawTradingUI(uint8_t cursor, CargoHold* playerHold, CargoHold* stationHold, SystemInfo* info);

void drawGameOverScreen();

void moveCursorDown(uint8_t* i, uint8_t max);
void moveCursorUp(uint8_t* i, uint8_t max);

#endif