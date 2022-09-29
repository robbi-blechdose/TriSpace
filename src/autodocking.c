#include "autodocking.h"

bool isAutodockPossible(Ship* ship, StarSystem* starSystem)
{
    return distance3d(&ship->position, &starSystem->station.dockingPosition) < DOCK_AUTO_DISTANCE;
}

void preCalcAutodockShip(AutodockData* ad, Ship* ship, StarSystem* starSystem)
{
    if(!isAutodockPossible(ship, starSystem))
    {
        return;
    }
    
    vec3 dock = starSystem->station.dockingPosition;

    ad->active = 1;
    ad->pointIndex = 0;
    ad->points[POINT_APPROACH] = (vec3) {.x = dock.x + 5 * DOCK_APPROACH_DISTANCE, .y = dock.y, .z = dock.z + 1.25f * DOCK_APPROACH_DISTANCE};
    ad->points[POINT_TARGET] = dock;
    //Calculate turn point based on ship position
    //Are we left or right of the "line" (target - approach)? (Use determinant of the vectors)
    float lr = (dock.x - ad->points[POINT_APPROACH].x) * (ship->position.z - ad->points[POINT_APPROACH].z) -
                (dock.z - ad->points[POINT_APPROACH].z) * (ship->position.x - ad->points[POINT_APPROACH].x);
    if(lr > 0)
    {
        ad->points[POINT_TURN] = (vec3) {.x = ad->points[POINT_APPROACH].x + 1.25f * 2 ,.y = dock.y, .z = ad->points[POINT_APPROACH].z - 5 * 2};
    }
    else
    {
        ad->points[POINT_TURN] = (vec3) {.x = ad->points[POINT_APPROACH].x - 1.25f * 2 ,.y = dock.y, .z = ad->points[POINT_APPROACH].z + 5 * 2};
    }
}

const float targetSpeeds[NUM_POINTS] = {
    [POINT_TURN] = 0.75f,
    [POINT_APPROACH] = 0.5f,
    [POINT_TARGET] = 0.25f
};

void calcAutodockShip(AutodockData* ad, Ship* ship, uint32_t ticks)
{
    float targetSpeed = shipTypes[ship->type].maxSpeed * targetSpeeds[ad->pointIndex];

    //Check if we're close to the point
    if(distance3d(&ship->position, &ad->points[ad->pointIndex]) < 2.1f)
    {
        if(ad->pointIndex == POINT_TARGET)
        {
            ad->active = 0;
            return;
        }
        ad->pointIndex++;
    }
    
    //Turn towards point
    turnShipTowardsPoint(ship, ad->points[ad->pointIndex]);

    if(fabs(ship->speed - targetSpeed) > 0.1f)
    {
        if(ship->speed < targetSpeed)
        {
            accelerateShip(ship, 1, ticks);
        }
        else
        {
            accelerateShip(ship, -1, ticks);
        }
    }
}