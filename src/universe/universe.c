#include "universe.h"

#include "starsystem.h"
#include "spacestation.h"
#include "generator.h"

uint16_t currentSystem;
uint32_t systemSeeds[256]; //16x16

void initUniverse(StarSystem* starSystem)
{
    initStarSystem();
    initSpaceStation();
    //Generate system seeds
    generateSystemSeeds(systemSeeds, 1);
    //Init starting star system
    currentSystem = 0;
    generateStarSystem(starSystem, systemSeeds[currentSystem]);
}

void switchSystem(uint16_t newSystem, StarSystem* starSystem)
{
    if(newSystem == currentSystem)
    {
        return;
    }
    currentSystem = newSystem;
    deleteStarSystem(starSystem);
    generateStarSystem(starSystem, systemSeeds[currentSystem]);
    //TODO: Clear NPC ships!
}

void calcNPCShips(Ship npcShips[])
{
    for(uint8_t i = 0; i < MAX_NPC_SHIPS; i++)
    {
        if(npcShips[i].type != NULL)
        {

        }
    }
}

void calcUniverse(State* state, State* targetState, StarSystem* starSystem, Ship* playerShip, Ship npcShips[])
{
    if(*targetState != NONE)
    {
        switch(*targetState)
        {
            case SPACE:
            {
                playerShip->position = starSystem->station.exitPosition;
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
            case TRADING:
            {
                playerShip->position.y = 0;
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
            if(hasDockingDistance(&playerShip->position, &starSystem->station.dockingPosition))
            {
                *targetState = STATION;
            }
            calcNPCShips(npcShips);
            break;
        }
        case STATION:
        {
            if(hasLeavingDistance(playerShip->position))
            {
                *targetState = SPACE;
            }
            else if(hasLandingDistance(playerShip->position))
            {
                *targetState = TRADING;
            }
            break;
        }
    }
}

void drawUniverse(State* state, StarSystem* starSystem, Ship npcShips[])
{
    switch(*state)
    {
        case SPACE:
        {
            drawStarSystem(starSystem);
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

uint32_t* getSystemSeeds()
{
    return systemSeeds;
}