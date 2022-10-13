#include "player.h"

#include "engine/savegame.h"

void savePlayer(Player* player)
{
    writeElement(&player->ship.type, sizeof(player->ship.type));
    writeElement(&player->ship.weapon.type, sizeof(player->ship.weapon.type));

    writeElement(&player->wantedLevel, sizeof(player->wantedLevel));
    writeElement(&player->killCount, sizeof(player->killCount));

    writeElement(&player->fuel, sizeof(player->fuel));
    writeElement(&player->hold, sizeof(player->hold));

    writeElement(&player->hasAutodock, sizeof(player->hasAutodock));
    writeElement(&player->hasFuelScoops, sizeof(player->hasFuelScoops));
}

void loadPlayer(Player* player)
{
    readElement(&player->ship.type, sizeof(player->ship.type));
    readElement(&player->ship.weapon.type, sizeof(player->ship.weapon.type));

    readElement(&player->wantedLevel, sizeof(player->wantedLevel));
    readElement(&player->killCount, sizeof(player->killCount));

    readElement(&player->fuel, sizeof(player->fuel));
    readElement(&player->hold, sizeof(player->hold));

    readElement(&player->hasAutodock, sizeof(player->hasAutodock));
    readElement(&player->hasFuelScoops, sizeof(player->hasFuelScoops));
}