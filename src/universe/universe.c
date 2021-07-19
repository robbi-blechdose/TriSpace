#include "universe.h"

#include "starsystem.h"
#include "spacestation.h"
#include "generator.h"

typedef enum {
    NONE,
    SPACE,
    STATION,
    PLANET
} State;

State state;
State targetState;

uint8_t currentSystem;
uint8_t systemSeeds[] = {
    2
};

#define MAX_NPC_SHIPS 8
Ship npcShips[8];

void initUniverse()
{
    state = SPACE;
    targetState = NONE;
    initStarSystem();
    initSpaceStation();
    //Init starting star system
    currentSystem = 0;
    generateStarSystem(getStarSystem(), systemSeeds[currentSystem]);

    //TODO: Remove test
    npcShips[0].type = 1;
    npcShips[0].position.x = 150;
    npcShips[0].position.z = 100;
}

void switchSystem(uint8_t newSystem)
{
    currentSystem = newSystem;
    deleteStarSystem();
    generateStarSystem(getStarSystem(), systemSeeds[currentSystem]);
    uint8_t i;
    for(i = 0; i < MAX_NPC_SHIPS; i++)
    {
        npcShips[i].type = NULL;
    }
}

void calcNPCShips()
{
    uint8_t i;
    for(i = 0; i < MAX_NPC_SHIPS; i++)
    {
        if(npcShips[i].type != NULL)
        {

        }
    }
}

void calcUniverse(Ship* playerShip)
{
    if(targetState != NONE)
    {
        switch(targetState)
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
        state = targetState;
        targetState = NONE;
    }

    switch(state)
    {
        case SPACE:
        {
            if(hasDockingDistance(playerShip))
            {
                targetState = STATION;
            }
            calcNPCShips();
            break;
        }
        case STATION:
        {
            if(hasLeavingDistance(playerShip))
            {
                targetState = SPACE;
            }
            break;
        }
    }
}

void drawUniverse()
{
    if(targetState != NONE)
    {
        //TODO: Visual transitions
        switch(targetState)
        {
            case SPACE:
            {
                break;
            }
            case STATION:
            {
                break;
            }
        }
    }

    switch(state)
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