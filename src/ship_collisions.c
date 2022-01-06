#include "ship_collisions.h"
#include <math.h>
#include "universe/asteroids.h"

uint8_t checkStarSystemCollision(Ship* ship, StarSystem* starSystem)
{
    //Basic collisions for planets
    for(uint8_t i = 0; i < starSystem->numPlanets; i++)
    {
        if(distance3d(&starSystem->planets[i].position, &ship->position) < starSystem->planets[i].size + 2)
        {
            return 1;
        }
    }
    //Basic collisions for stars
    for(uint8_t i = 0; i < starSystem->numStars; i++)
    {
        if(distance3d(&starSystem->stars[i].position, &ship->position) < starSystem->stars[i].size + 2)
        {
            return 1;
        }
    }
    //Basic collisions with the space station
    if(distance3d(&starSystem->station.position, &ship->position) < 6.75f && fabs(starSystem->station.position.y - ship->position.y) < 2.4f)
    {
        return 1;
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
                return 1;
            }
        }
    }
    return 0;
}

uint8_t checkStationCollision(Ship* ship)
{
    return ship->position.y > 0.6f || //We don't need this since it triggers the GUI: ship->position.y < -1.4f
            ship->position.z < -1.8f || ship->position.z > 1.8f ||
            ship->position.x < -2.4f; //The other direction is leaving the station, so we don't need it
}