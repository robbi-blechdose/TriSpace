#ifndef PLAYER_H
#define PLAYER_H

#include <stdint.h>

#include "ship.h"
#include "cargo.h"

typedef struct {
    Ship ship;

    uint8_t wantedLevel;
    uint32_t killCount;

    float fuel;

    CargoHold hold;

    bool hasAutodock;
    bool hasFuelScoops;

    bool fuelScoopsActive;
} Player;

#define WANTED_LEVEL_DANGEROUS  7
#define MAX_WANTED_LEVEL       10

void savePlayer(Player* player);
void loadPlayer(Player* player);

#endif