#ifndef _UI_H
#define _UI_H

#include "ship.h"
#include "universe/universe.h"
#include "contracts.h"

#define RADAR_RANGE 100

//Macro for centering text
//X is the size of the text in characters
//X * 8 / 2 = X * 4
#define CENTER(X) (120 - (X) * 4)

#define NUM_EQUIPMENT 2

#define EQUIP_FUEL 0
#define EQUIP_HOLD30 1
#define EQUIP_MK2LASER 2

void initUI();

void drawUI(State state, Ship* playerShip, Ship npcShips[], vec3 stationPos, uint8_t displayContract, vec3 contractPos);

void drawSaveLoadUI(uint8_t cursor);

void drawTradingUI(CargoHold* playerHold, CargoHold* stationHold, SystemInfo* info);
void moveTradeCursor(int8_t dir);
uint8_t getTradeCursor();

void drawEquipUI(Ship* playerShip);
void moveEquipCursor(int8_t dir);
uint8_t getEquipCursor();

void drawContractUI(Contract* activeContract, Contract* contracts, uint8_t numContracts);
void resetContractCursor();
void moveContractCursor(int8_t dir, uint8_t numContracts);
uint8_t getContractCursor();

void drawMap(uint8_t currentSystem[2], float fuel);
void moveMapCursor(int8_t x, int8_t y);
void getMapCursor(uint8_t mapCursor[2]);

void drawTitleScreen();
void toggleTitleCursor();
uint8_t getTitleCursor();

#endif