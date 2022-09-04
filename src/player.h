#ifndef _PLAYER_H
#define _PLAYER_H

#include <stdint.h>

#include "ship.h"
#include "cargo.h"

typedef struct {
    Ship ship;

    uint8_t wantedLevel;

    float fuel;

    CargoHold hold;

    bool hasAutodock;
    bool hasFuelScoops;

    bool fuelScoopsActive;
} Player;

void savePlayer(Player* player);
void loadPlayer(Player* player);

#endif