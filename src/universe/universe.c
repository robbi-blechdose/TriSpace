#include "universe.h"

#include "starsystem.h"
#include "spacestation.h"
#include "generator.h"
#include "../engine/effects.h"
#include "../engine/util.h"
#include "../shipAi.h"

uint32_t systemSeeds[UNIVERSE_SIZE][UNIVERSE_SIZE];

void initUniverse(uint8_t* currentSystem, StarSystem* starSystem)
{
    initEffects();
    initStarSystem();
    initSpaceStation();
    //Generate system seeds
    generateSystemSeeds(systemSeeds, BASE_SEED);
    //Init starting star system
    currentSystem[0] = 0;
    currentSystem[1] = 0;
    generateStarSystem(starSystem, systemSeeds[currentSystem[0]][currentSystem[1]]);
}

void generateNPCShips(Ship npcShips[], uint8_t maxShips, StarSystem* starSystem)
{
    uint8_t numShips = randr(maxShips);

    for(uint8_t i = 0; i < numShips; i++)
    {
        npcShips[i].type = SHIP_TYPE_SMALLPIRATE;
        //Relatively safe system
        if((MAX_GOVERNMENT - starSystem->info.government) < (MAX_GOVERNMENT / 2))
        {
            if(randr(100) < 55)
            {
                npcShips[i].type = SHIP_TYPE_POLICE;
            }
        }
        npcShips[i].weapon.type = 0; //TODO: Randomize a bit
        vec3 pos = getRandomFreePos(starSystem, 10);
        npcShips[i].position.x = pos.x;
        npcShips[i].position.z = pos.z;
        npcShips[i].position.y = pos.y;
    }
}

void switchSystem(uint8_t* currentSystem, uint8_t newSystem[2], StarSystem* starSystem, Ship npcShips[])
{
    if(newSystem[0] == currentSystem[0] && newSystem[1] == currentSystem[1])
    {
        return;
    }
    currentSystem[0] = newSystem[0];
    currentSystem[1] = newSystem[1];
    deleteStarSystem(starSystem);
    generateStarSystem(starSystem, systemSeeds[currentSystem[0]][currentSystem[1]]);
    for(uint8_t i = 0; i < MAX_NPC_SHIPS; i++)
    {
        npcShips[i].type = SHIP_TYPE_NULL;
    }
    generateNPCShips(npcShips, NUM_NORM_NPC_SHIPS, starSystem);
}

void calcNPCShips(Ship* playerShip, Ship npcShips[], StarSystem* starSystem, uint32_t ticks)
{
    for(uint8_t i = 0; i < MAX_NPC_SHIPS; i++)
    {
        if(npcShips[i].type != SHIP_TYPE_NULL)
        {
            calcNPCAi(playerShip, &npcShips[i], ticks);
            //TODO: Collisions?
            calcShip(&npcShips[i], 0, ticks);
            if(shipIsDestroyed(&npcShips[i]))
            {
                createEffect(npcShips[i].position, EXPLOSION);
                npcShips[i].type = SHIP_TYPE_NULL;
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
                playerShip->speed *= 0.5f;
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
                if(npcShips[i].type != SHIP_TYPE_NULL)
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

uint32_t getSeedForSystem(uint8_t x, uint8_t y)
{
    return systemSeeds[x][y];
}

//Converts from menu distances (64 units between systems on average) to light years
#define MENU_TO_LIGHTYEARS(X) ((X) / (64.0f / 3.0f))

float getDistanceToSystem(uint8_t currentSystem[2], uint8_t targetSystem[2])
{
    vec2 currentPos;
    vec2 targetPos;
    generateSystemPos(&currentPos, systemSeeds[currentSystem[0]][currentSystem[1]], currentSystem[0], currentSystem[1]);
    generateSystemPos(&targetPos, systemSeeds[targetSystem[0]][targetSystem[1]], targetSystem[0], targetSystem[1]);
    return MENU_TO_LIGHTYEARS(distance2d(&currentPos, &targetPos));
}