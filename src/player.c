#include "player.h"

#include "fk-engine-core/savegame.h"

void savePlayer(Player* player)
{
    writeElement(player, sizeof(Player));
}

void loadPlayer(Player* player)
{
    readElement(player, sizeof(Player));
}