#include "universe.h"

#include "../engine/effects.h"
#include "../engine/audio.h"
#include "../engine/util.h"

#include "asteroids.h"
#include "satellites.h"
#include "generator.h"

#include "../npcs/ai.h"

uint32_t systemSeeds[UNIVERSE_SIZE][UNIVERSE_SIZE];

uint8_t sampleExplosion;

vec3 lastPlayerPos;

void initUniverse(StarSystem* starSystem)
{
    sampleExplosion = loadSample("res/sfx/explosion.wav");
    //Generate system seeds
    srand(BASE_SEED);
    for(uint8_t i = 0; i < UNIVERSE_SIZE; i++)
    {
        for(uint8_t j = 0; j < UNIVERSE_SIZE; j++)
        {
            systemSeeds[i][j] = rand();
        }
    }
}

void initSystem(uint8_t* currentSystem, StarSystem* starSystem, Npc npcs[])
{
    generateStarSystem(starSystem, systemSeeds[currentSystem[0]][currentSystem[1]]);
    //Clear NPC ships
    for(uint8_t i = 0; i < MAX_NPCS; i++)
    {
        npcs[i].ship.type = SHIP_TYPE_NULL;
    }
    //Create asteroids for system (if any)
    if(starSystem->hasAsteroidField)
    {
        createAsteroids(starSystem->asteroidFieldPos);
    }
}

void switchSystem(uint8_t* currentSystem, uint8_t newSystem[2], StarSystem* starSystem, Npc npcs[])
{
    if(newSystem[0] == currentSystem[0] && newSystem[1] == currentSystem[1])
    {
        return;
    }
    currentSystem[0] = newSystem[0];
    currentSystem[1] = newSystem[1];
    deleteStarSystem(starSystem);
    clearSatellites();
    initSystem(currentSystem, starSystem, npcs);
}

void calcNPCShips(StarSystem* starSystem, Player* player, Npc npcs[], uint32_t ticks)
{
    for(uint8_t i = 0; i < MAX_NPCS; i++)
    {
        if(npcs[i].ship.type != SHIP_TYPE_NULL)
        {
            calcNPCAi(&npcs[i], player, npcs, ticks);
            //TODO: Collisions?
            calcShip(&npcs[i].ship, 0, ticks);
            if(shipIsDestroyed(&npcs[i].ship))
            {
                playSample(sampleExplosion);
                createEffect(npcs[i].ship.position, EXPLOSION);
                npcs[i].ship.type = SHIP_TYPE_NULL;
            }
        }
    }
}

void generateNPCShips(Npc npcs[], uint8_t maxShips, StarSystem* starSystem, vec3 center)
{
    for(uint8_t i = 0; i < NUM_NORM_NPCS; i++)
    {
        if(npcs[i].ship.type == SHIP_TYPE_NULL)
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
                npcs[i].ship.type = shipType;
                npcs[i].ship.weapon.type = 0; //TODO: Randomize a bit
                npcs[i].ship.shields = shipTypes[shipType].maxShields;
                vec3 pos = getRandomFreePosBounds(starSystem, center, (vec3) {.x = 80, .y = 50, .z = 80}, 10, 30);
                npcs[i].ship.position.x = pos.x;
                npcs[i].ship.position.z = pos.z;
                npcs[i].ship.position.y = pos.y;
            }
        }
    }
}

void setInitialSpawnPos(vec3 playerPos)
{
    lastPlayerPos = playerPos;
}

void calcUniverseSpawnNPCShips(StarSystem* starSystem, Ship* playerShip, Npc npcs[], uint32_t ticks)
{
    if(distance3d(&lastPlayerPos, &playerShip->position) > SPAWN_INTERVAL_DISTANCE)
    {
        //Generate NPC ships
        generateNPCShips(npcs, NUM_NORM_NPCS, starSystem, playerShip->position);
        //Store position for next run
        lastPlayerPos = playerShip->position;
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