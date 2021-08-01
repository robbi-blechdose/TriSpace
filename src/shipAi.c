#include "shipAi.h"

void calcNPCAi(Ship* playerShip, Ship* npcShip, uint32_t ticks)
{
    float distance = distance3d(&playerShip->position, &npcShip->position);

    if(distance < AI_RANGE)
    {
        if(distance > 10 && npcShip->speed < npcShip->type->maxSpeed - 1)
        {
            //TODO: Calc direction towards player
            accelerateShip(npcShip, 1, ticks);
        }
    }
    else
    {
        accelerateShip(npcShip, -1, ticks);
    }
}