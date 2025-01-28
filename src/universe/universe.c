#include "universe.h"

#include "../engine/effects.h"
#include "../fk-engine-core/audio.h"
#include "../fk-engine-core/util.h"

#include "asteroids.h"
#include "satellites.h"
#include "generator/generator.h"
#include "../ship_collisions.h"
#include "../npcs/ai.h"

uint32_t systemSeeds[UNIVERSE_SIZE][UNIVERSE_SIZE];

uint8_t sampleExplosion;

vec3 lastPlayerPos;

void initUniverse()
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

void initSystem(int8_t* currentSystem, StarSystem* system, Npc npcs[])
{
    generateStarSystem(system, systemSeeds[currentSystem[0]][currentSystem[1]]);
    //Clear NPC ships
    for(uint8_t i = 0; i < MAX_NPCS; i++)
    {
        npcs[i].ship.type = SHIP_TYPE_NULL;
    }
    //Create asteroids for system (if any)
    if(system->info.hasAsteroidField)
    {
        createAsteroids(system->asteroidFieldPos);
    }
}

void switchSystem(int8_t* currentSystem, int8_t newSystem[2], StarSystem* system, Npc npcs[])
{
    if(newSystem[0] == currentSystem[0] && newSystem[1] == currentSystem[1])
    {
        return;
    }
    currentSystem[0] = newSystem[0];
    currentSystem[1] = newSystem[1];
    deleteStarSystem(system);
    clearSatellites();
    initSystem(currentSystem, system, npcs);
}

void calcNPCShips(StarSystem* system, Player* player, Npc npcs[], uint32_t ticks)
{
    for(uint8_t i = 0; i < MAX_NPCS; i++)
    {
        if(npcs[i].ship.type != SHIP_TYPE_NULL)
        {
            calcNPCAi(&npcs[i], player, npcs, ticks);
            calcShip(&npcs[i].ship, ticks);

            if(checkStarSystemCollision(&npcs[i].ship, system))
            {
                npcs[i].ship.shields = -1;
            }

            if(shipIsDestroyed(&npcs[i].ship))
            {
                playSample(sampleExplosion);
                createEffect(npcs[i].ship.position, EXPLOSION);
                npcs[i].ship.type = SHIP_TYPE_NULL;
                player->killCount++;
            }
        }
    }
}

void generateNPCShips(Npc npcs[], uint8_t maxShips, StarSystem* system, vec3 center)
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
            uint8_t enemyChance = (MAX_GOVERNMENT * 100 - system->info.characteristics.government * 100) / MAX_GOVERNMENT;
            uint8_t policeChance = 100 - enemyChance;
            uint8_t shipType = SHIP_TYPE_NULL;

            if(randr(100) < enemyChance)
            {
                uint8_t rand2 = randr(100);
                if(rand2 < 35)
                {
                    shipType = SHIP_TYPE_SMALLPIRATE;
                }
                else if(rand2 < 70)
                {
                    shipType = SHIP_TYPE_SMALLPIRATE2;
                }
                else
                {
                    shipType = SHIP_TYPE_SPHERE_PIRATE;
                }
            }
            else if(randr(100) < policeChance)
            {
                shipType = SHIP_TYPE_POLICE;
            }
            else if(randr(100) < 5)
            {
                shipType = SHIP_TYPE_ALIEN;
            }

            if(shipType != SHIP_TYPE_NULL)
            {
                vec3 pos = getRandomFreePosBounds(system, center, (vec3) {.x = 80, .y = 50, .z = 80}, 10, 30);
                npcs[i].ship = (Ship) {.type = shipType,
                                       .weapon.type = 0, //TODO: Randomize a bit
                                       .shields = shipTypes[shipType].maxShields,
                                       .position = pos,
                                       .rotation = QUAT_INITIAL};
            }
        }
    }
}

void setInitialSpawnPos(vec3 playerPos)
{
    lastPlayerPos = playerPos;
}

void calcUniverseSpawnNPCShips(StarSystem* system, Ship* playerShip, Npc npcs[], uint32_t ticks)
{
    if(distance3d(&lastPlayerPos, &playerShip->position) > SPAWN_INTERVAL_DISTANCE)
    {
        //Generate NPC ships
        generateNPCShips(npcs, NUM_NORM_NPCS, system, playerShip->position);
        //Store position for next run
        lastPlayerPos = playerShip->position;
    }
}

uint32_t getSeedForSystem(int8_t x, int8_t y)
{
    return systemSeeds[x][y];
}

float getDistanceToSystem(int8_t currentSystem[2], int8_t targetSystem[2])
{
    vec2 currentPos = generateSystemPos(systemSeeds[currentSystem[0]][currentSystem[1]], currentSystem[0], currentSystem[1]);
    vec2 targetPos = generateSystemPos(systemSeeds[targetSystem[0]][targetSystem[1]], targetSystem[0], targetSystem[1]);
    return distance2d(&currentPos, &targetPos);
}