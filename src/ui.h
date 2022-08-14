#ifndef _UI_H
#define _UI_H

#include <stdbool.h>

#include "ship.h"
#include "universe/universe.h"
#include "contracts.h"

#define RADAR_RANGE 120

//Macro for centering text
//X is the size of the text in characters
//X * 8 / 2 = X * 4
#define CENTER(X) (120 - (X) * 4)

void initUI();

/**
 * Functions for a popup window
 * Can hold 15x6 characters, plus an icon
 **/

#define POPUP_CHECKMARK 0
#define POPUP_ATTENTION 1

void drawPopupIfActive();
void createPopup(uint8_t icon, char* text);
void closePopup();
bool isPopupOpen();

void drawUI(State state, Ship* playerShip, Ship npcShips[], vec3 stationPos, uint8_t autodockPossible);

void drawSaveLoadUI(uint8_t cursor);

void drawTradingUI(uint8_t cursor, CargoHold* playerHold, CargoHold* stationHold, SystemInfo* info);

void drawEquipUI(uint8_t cursor, Ship* playerShip);

void drawContractUI(uint8_t cursor, Contract* activeContract, Contract* contracts, uint8_t numContracts);

void drawMap(uint8_t cursor[2], uint8_t currentSystem[2], float fuel);

void drawTitleScreen(uint8_t cursor);

void drawGameOverScreen();

void moveCursorDown(uint8_t* i, uint8_t max);
void moveCursorUp(uint8_t* i, uint8_t max);

#endif