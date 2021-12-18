#include "autodocking.h"

uint8_t isAutodockPossible(Ship* ship, StarSystem* starSystem)
{
    return distance3d(&ship->position, &starSystem->station.dockingPosition) < DOCK_AUTO_DISTANCE;
}

void preCalcAutodockShip(AutodockData* ad, Ship* ship, StarSystem* starSystem)
{
    if(isAutodockPossible(ship, starSystem))
    {
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

        /**
        for(uint8_t i = 0; i < NUM_POINTS; i++)
        {
            printf("%d: %f %f %f\n", i, ad->points[i].x, ad->points[i].y, ad->points[i].z);
        }**/
    }
}

void calcAutodockShip(AutodockData* ad, Ship* ship, uint32_t ticks)
{
    float targetX, targetY;
    float targetSpeed;
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
    calcRotToTarget(&ship->position, &ad->points[ad->pointIndex], &targetY, &targetX);
    //TODO
    //Set target speed
    switch(ad->pointIndex)
    {
        case POINT_TURN:
        {
            targetSpeed = shipTypes[ship->type].maxSpeed;
            break;
        }
        case POINT_APPROACH:
        {
            targetSpeed = shipTypes[ship->type].maxSpeed * 0.5f;
            break;
        }
        case POINT_TARGET:
        {
            targetSpeed = shipTypes[ship->type].maxSpeed * 0.25f;
            break;
        }
    }
    //Apply target rotation and speed
    ship->turnSpeedX = getTurnSpeedForRotation(ship->rotation.x, targetX, shipTypes[ship->type].maxTurnSpeed / 2);
    ship->turnSpeedY = getTurnSpeedForRotation(ship->rotation.y, targetY, shipTypes[ship->type].maxTurnSpeed / 2);

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