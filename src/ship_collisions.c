#include "ship_collisions.h"

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
    for(uint8_t i = 0; i < starSystem->numStars; i++)
    {
        if(distance3d(&starSystem->stars[i].position, &ship->position) < starSystem->stars[i].size + 2)
        {
            return 1;
        }
    }
    //TODO: Check station
    return 0;
}

uint8_t checkStationCollision(Ship* ship)
{
    return ship->position.y > 0.6f || //We don't need this since it triggers the GUI: ship->position.y < -1.4f
            ship->position.z < -1.8f || ship->position.z > 1.8f ||
            ship->position.x < -2.4f; //The other direction is leaving the station, so we don't need it
}