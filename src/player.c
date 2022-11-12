#include "player.h"

#include "engine/savegame.h"

void savePlayer(Player* player)
{
    writeElement(player, sizeof(Player));
}

void loadPlayer(Player* player)
{
    readElement(player, sizeof(Player));
}