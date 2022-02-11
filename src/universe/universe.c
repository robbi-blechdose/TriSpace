#include "universe.h"

#include "starsystem.h"
#include "spacestation.h"
#include "asteroids.h"
#include "generator.h"
#include "../engine/effects.h"
#include "../engine/audio.h"
#include "../engine/util.h"
#include "../shipAi.h"
#include "satellites.h"

uint32_t systemSeeds[UNIVERSE_SIZE][UNIVERSE_SIZE];

uint8_t sampleExplosion;

vec3 lastPlayerPos;

void initUniverse(uint8_t* currentSystem, StarSystem* starSystem, Ship npcShips[])
{
    initEffects();
    initStarSystem();
    initSpaceStation();
    initAsteroids();
    initSatellites();
    sampleExplosion = loadSample("res/sfx/explosion.wav");
    //Generate system seeds
    generateSystemSeeds(systemSeeds, BASE_SEED);
    //Init starting star system
    currentSystem[0] = 0;
    currentSystem[1] = 0;
}

void initSystem(uint8_t* currentSystem, StarSystem* starSystem, Ship npcShips[])
{
    generateStarSystem(starSystem, systemSeeds[currentSystem[0]][currentSystem[1]]);
    //Clear NPC ships
    for(uint8_t i = 0; i < MAX_NPC_SHIPS; i++)
    {
        npcShips[i].type = SHIP_TYPE_NULL;
    }
    //Create asteroids for system (if any)
    if(starSystem->hasAsteroidField)
    {
        createAsteroids(starSystem->asteroidFieldPos);
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
    clearSatellites();
    initSystem(currentSystem, starSystem, npcShips);
}

void calcNPCShips(StarSystem* starSystem, Ship* playerShip, Ship npcShips[], uint32_t ticks)
{
    for(uint8_t i = 0; i < MAX_NPC_SHIPS; i++)
    {
        if(npcShips[i].type != SHIP_TYPE_NULL)
        {
            calcNPCAi(playerShip, &npcShips[i], npcShips, ticks);
            //TODO: Collisions?
            calcShip(&npcShips[i], 0, ticks);
            if(shipIsDestroyed(&npcShips[i]))
            {
                playSample(sampleExplosion);
                createEffect(npcShips[i].position, EXPLOSION);
                npcShips[i].type = SHIP_TYPE_NULL;
            }
        }
    }
}

void generateNPCShips(Ship npcShips[], uint8_t maxShips, StarSystem* starSystem, vec3 center)
{
    for(uint8_t i = 0; i < NUM_NORM_NPC_SHIPS; i++)
    {
        if(npcShips[i].type == SHIP_TYPE_NULL)
        {
            if(randr(100) > SPAWN_CHANCE)
            {
                continue;
            }

            //Ship type
            uint8_t enemyChance = (MAX_GOVERNMENT * 100 - starSystem->info.government * 100) / MAX_GOVERNMENT;
            uint8_t policeChance = 100 - enemyChance;
            uint8_t shipType = SHIP_TYPE_NULL;

            if(randr(100) < enemyChance)
            {
                shipType = SHIP_TYPE_SMALLPIRATE;
            }
            else if(randr(100) < policeChance)
            {
                shipType = SHIP_TYPE_POLICE;
            }

            if(shipType != SHIP_TYPE_NULL)
            {
                npcShips[i].type = shipType;
                npcShips[i].weapon.type = 0; //TODO: Randomize a bit
                npcShips[i].shields = shipTypes[shipType].maxShields;
                vec3 pos = getRandomFreePosBounds(starSystem, center, (vec3) {.x = 80, .y = 50, .z = 80}, 10, 30);
                npcShips[i].position.x = pos.x;
                npcShips[i].position.z = pos.z;
                npcShips[i].position.y = pos.y;
            }
        }
    }
}

void setInitialSpawnPos(vec3 playerPos)
{
    lastPlayerPos = playerPos;
}

void calcUniverseSpawnNPCShips(StarSystem* starSystem, Ship* playerShip, Ship npcShips[], uint32_t ticks)
{
    if(distance3d(&lastPlayerPos, &playerShip->position) > SPAWN_INTERVAL_DISTANCE)
    {
        //Generate NPC ships
        generateNPCShips(npcShips, NUM_NORM_NPC_SHIPS, starSystem, playerShip->position);
        //Store position for next run
        lastPlayerPos = playerShip->position;
    }
}

void calcUniverse(State* state, StarSystem* starSystem, Ship* playerShip, Ship npcShips[], uint32_t ticks)
{
    switch(*state)
    {
        case SPACE:
        {
            if(hasSatellites())
            {
                checkVisitSatellite(&playerShip->position);
            }
            calcUniverseSpawnNPCShips(starSystem, playerShip, npcShips, ticks);
            calcNPCShips(starSystem, playerShip, npcShips, ticks);
            calcEffects(ticks);
            if(hasDockingDistance(&playerShip->position, &starSystem->station.dockingPosition))
            {
                *state = STATION;
                playerShip->position.x = 2;
                playerShip->position.y = 0;
                playerShip->position.z = 0;
                playerShip->speed *= 0.5f;
            }
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
        case GAME_OVER:
        {
            drawStarSystem(starSystem);
            if(starSystem->hasAsteroidField)
            {
                drawAsteroids();
            }
            for(uint8_t i = 0; i < MAX_NPC_SHIPS; i++)
            {
                if(npcShips[i].type != SHIP_TYPE_NULL)
                {
                    drawShip(&npcShips[i]);
                }
            }
            drawSatellites();
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