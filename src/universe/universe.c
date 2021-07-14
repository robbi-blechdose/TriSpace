#include "universe.h"

#include "starsystem.h"
#include "spacestation.h"

typedef enum {
    NONE,
    SPACE,
    STATION,
    PLANET
} State;

State state;
State targetState;

void initUniverse()
{
    state = SPACE;
    targetState = NONE;
    initStarSystem();
    initSpaceStation();
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
            break;
        }
        case STATION:
        {
            drawSpaceStation();
            break;
        }
    }
}