#include "ai.h"

//Friendly: set direction, move at half speed, randomly change direction (small chance)
//Police: random checks for illegal cargo, attack if true, attack if contract ship is damaged by player, attack if shot by player, otherwise patrol
//Enemy: attack when in range

void calcNPCAi(Npc* npc, Player* player, Npc* npcs, uint32_t ticks)
{
    switch(npc->ship.type)
    {
        case SHIP_TYPE_SMALLPIRATE:
        {
            break;
        }
        case SHIP_TYPE_CRUISELINER:
        {
            break;
        }
        case SHIP_TYPE_POLICE:
        {
            break;
        }
    }
}