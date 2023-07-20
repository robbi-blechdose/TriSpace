#include "ship_collisions.h"
#include <math.h>
#include "universe/asteroids.h"

bool checkStarSystemCollision(Ship* ship, StarSystem* starSystem)
{
    //Basic collisions for planets
    for(uint8_t i = 0; i < starSystem->info.numPlanets; i++)
    {
        if(distance3d(&starSystem->planets[i].position, &ship->position) < starSystem->planets[i].size + 2)
        {
            return true;
        }
    }
    //Basic collisions for stars
    for(uint8_t i = 0; i < starSystem->info.numStars; i++)
    {
        if(distance3d(&starSystem->stars[i].position, &ship->position) < starSystem->stars[i].size + 2)
        {
            return true;
        }
    }
    //Basic collisions with the space station
    if(distance3d(&starSystem->station.position, &ship->position) < 6.75f && fabs(starSystem->station.position.y - ship->position.y) < 2.4f)
    {
        return true;
    }
    //Basic collisions for asteroids
    //Preliminary check (are we near the asteroid field?)
    if(distance3d(&starSystem->asteroidFieldPos, &ship->position) < ASTEROID_FIELD_SIZE + 10)
    {
        for(uint8_t i = 0; i < NUM_ASTEROIDS; i++)
        {
            Asteroid asteroid = getAsteroids()[i];
            if(distance3d(&asteroid.position, &ship->position) < asteroid.size + 2)
            {
                return true;
            }
        }
    }
    return false;
}

bool checkStationCollision(Ship* ship)
{
    return ship->position.y > 1.0f || //We don't need this since it triggers the GUI: ship->position.y < -1.4f
            ship->position.z < -3.1f || ship->position.z > 3.1f ||
            ship->position.x < -3.3f; //The other direction is leaving the station, so we don't need it
}