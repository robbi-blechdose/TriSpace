#include "generator.h"
#include <stdlib.h>

uint32_t randr(uint32_t max)
{
    return rand() / (RAND_MAX / max + 1);
}

float randf(float max)
{
    return ((float)rand() / (float)(RAND_MAX)) * max;
}

void generateStarSystem(StarSystem* system, uint32_t seed)
{
    srand(seed);

    system->numStars = randr(4);
    system->numPlanets = randr(9);

    float baseStarSize = 30 + randf(50);

    uint8_t i;
    for(i = 0; i < system->numStars; i++)
    {
        system->stars[i].size = baseStarSize + randf(5);
        system->stars[i].position.x = 0;
        system->stars[i].position.y = 0;
        system->stars[i].position.z = 0;
    }

    for(i = 0; i < system->numPlanets; i++)
    {
        system->planets[i].size = 10.0f + randf(10);
        system->planets[i].position.x = 20 * i + baseStarSize + randf(50 * i);
        system->planets[i].position.y = randf(5 * i);
        system->planets[i].position.z = 20 * i + baseStarSize + randf(50 * i);
    }

    //Index of the planets we're putting the station next to
    uint8_t spIndex = randr(system->numPlanets);
    system->station.position.x = system->planets[spIndex].position.x + 20;
    system->station.position.y = system->planets[spIndex].position.y;
    system->station.position.z = system->planets[spIndex].position.z;
    system->station.dockingPosition.x = system->station.position.x + 5;
    system->station.dockingPosition.y = system->station.position.y;
    system->station.dockingPosition.z = system->station.position.z + 1.25f;
}