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

void drawSaveLoadUI();
void toggleSaveLoadCursor();
uint8_t getSaveLoadCursor();

void drawTradingUI(CargoHold* playerHold, CargoHold* stationHold, SystemInfo* info);
void moveTradeCursor(int8_t dir);
uint8_t getTradeCursor();

void drawEquipUI(Ship* playerShip);
void moveEquipCursor(int8_t dir);
uint8_t getEquipCursor();

void drawContractUI(Contract* activeContract, Contract* contracts, uint32_t systemSeeds[], uint8_t numContracts);
void moveContractCursor(int8_t dir, uint8_t numContracts);
uint8_t getContractCursor();

void drawMap(uint32_t systemSeeds[]);
void moveMapCursor(int8_t x, int8_t y);
uint16_t getMapCursor();

void drawTitleScreen();
void toggleTitleCursor();
uint8_t getTitleCursor();

#endif