#include "universe.h"

#include "starsystem.h"
#include "spacestation.h"
#include "generator.h"

uint8_t currentSystem;
uint8_t systemSeeds[] = {
    2
};

void initUniverse()
{
    initStarSystem();
    initSpaceStation();
    //Init starting star system
    currentSystem = 0;
    generateStarSystem(getStarSystem(), systemSeeds[currentSystem]);
}

void switchSystem(uint8_t newSystem)
{
    currentSystem = newSystem;
    deleteStarSystem();
    generateStarSystem(getStarSystem(), systemSeeds[currentSystem]);
    uint8_t i;
    for(i = 0; i < MAX_NPC_SHIPS; i++)
    {
        //npcShips[i].type = NULL;
    }
}

void calcNPCShips(Ship npcShips[])
{
    uint8_t i;
    for(i = 0; i < MAX_NPC_SHIPS; i++)
    {
        if(npcShips[i].type != NULL)
        {

        }
    }
}

void calcUniverse(State* state, State* targetState, Ship* playerShip, Ship npcShips[])
{
    if(*targetState != NONE)
    {
        switch(*targetState)
        {
            case SPACE:
            {
                playerShip->position = getExitPosition();
                break;
            }
            case STATION:
            {
                playerShip->position.x = 0;
                playerShip->position.y = 0;
                playerShip->position.z = 0;
                playerShip->speed = 0;
                break;
            }
        }
        *state = *targetState;
        *targetState = NONE;
    }

    switch(*state)
    {
        case SPACE:
        {
            if(hasDockingDistance(playerShip))
            {
                *targetState = STATION;
            }
            calcNPCShips(npcShips);
            break;
        }
        case STATION:
        {
            if(hasLeavingDistance(playerShip))
            {
                *targetState = SPACE;
            }
            break;
        }
    }
}

void drawUniverse(State* state, Ship npcShips[])
{
    switch(*state)
    {
        case SPACE:
        {
            drawStarSystem();
            uint8_t i;
            for(i = 0; i < MAX_NPC_SHIPS; i++)
            {
                if(npcShips[i].type != NULL)
                {
                    drawShip(&npcShips[i]);
                }
            }
            break;
        }
        case STATION:
        {
            drawSpaceStation();
            break;
        }
    }
}