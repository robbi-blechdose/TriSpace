#include "universe.h"

#include "starsystem.h"
#include "spacestation.h"
#include "generator.h"
#include "../engine/effects.h"
#include "../engine/util.h"
#include "../shipAi.h"

uint16_t currentSystem;
uint32_t systemSeeds[256]; //16x16

void initUniverse(StarSystem* starSystem)
{
    initEffects();
    initStarSystem();
    initSpaceStation();
    //Generate system seeds
    generateSystemSeeds(systemSeeds, BASE_SEED);
    //Init starting star system
    currentSystem = 0;
    generateStarSystem(starSystem, systemSeeds[currentSystem]);
}

void switchSystem(uint16_t newSystem, StarSystem* starSystem, Ship npcShips[])
{
    if(newSystem == currentSystem)
    {
        return;
    }
    currentSystem = newSystem;
    deleteStarSystem(starSystem);
    generateStarSystem(starSystem, systemSeeds[currentSystem]);
    for(uint8_t i = 0; i < MAX_NPC_SHIPS; i++)
    {
        npcShips[i].type = TYPE_NULL;
    }
    generateNPCShips(npcShips, MAX_NPC_SHIPS, starSystem);
}

void calcNPCShips(Ship* playerShip, Ship npcShips[], StarSystem* starSystem, uint32_t ticks)
{
    for(uint8_t i = 0; i < MAX_NPC_SHIPS; i++)
    {
        if(npcShips[i].type != TYPE_NULL)
        {
            calcNPCAi(playerShip, &npcShips[i], ticks);
            //TODO: Collisions?
            calcShip(&npcShips[i], 0, ticks);
            if(shipIsDestroyed(&npcShips[i]))
            {
                createEffect(npcShips[i].position, EXPLOSION);
                npcShips[i].type = TYPE_NULL;
            }
        }
    }
}

void calcUniverse(State* state, StarSystem* starSystem, Ship* playerShip, Ship npcShips[], uint32_t ticks)
{
    switch(*state)
    {
        case SPACE:
        {
            if(hasDockingDistance(&playerShip->position, &starSystem->station.dockingPosition))
            {
                *state = STATION;
                playerShip->position.x = 0;
                playerShip->position.y = 0;
                playerShip->position.z = 0;
                playerShip->speed = 0;
            }
            calcNPCShips(playerShip, npcShips, starSystem, ticks);
            calcEffects(ticks);
            break;
        }
        case STATION:
        {
            if(hasLeavingDistance(playerShip->position))
            {
                *state = SPACE;
                playerShip->position = starSystem->station.exitPosition;
            }
            else if(hasLandingDistance(playerShip->position))
            {
                *state = TRADING;
                playerShip->position.y = 0;
                playerShip->speed = 0;
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
        case HYPERSPACE:
        {
            drawStarSystem(starSystem);
            for(uint8_t i = 0; i < MAX_NPC_SHIPS; i++)
            {
                if(npcShips[i].type != TYPE_NULL)
                {
                    drawShip(&npcShips[i]);
                }
            }
            drawEffects();
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

uint16_t getCurrentSystem()
{
    return currentSystem;
}

//Converts from menu distances (64 units between systems on average) to light years
#define MENU_TO_LIGHTYEARS(X) ((X) / (64.0f / 3.0f))

float getDistanceToSystem(uint16_t targetSystem)
{
    uint8_t x1 = currentSystem % 16;
    uint8_t y1 = currentSystem == 0 ? 0 : currentSystem / 16;
    uint8_t x2 = targetSystem % 16;
    uint8_t y2 = targetSystem == 0 ? 0 : targetSystem / 16;
    vec2 currentPos;
    vec2 targetPos;
    generateSystemPos(&currentPos, systemSeeds[currentSystem], x1, y1);
    generateSystemPos(&targetPos, systemSeeds[targetSystem], x2, y2);
    return MENU_TO_LIGHTYEARS(distance2d(&currentPos, &targetPos));
}