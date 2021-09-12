#include "shipAi.h"

void calcRotToTarget(vec3* pos, vec3* target, float* yRot, float* xRot)
{
    vec3 diff = subv3(*pos, *target);
    diff = normalizev3(diff);

    *yRot = atan2f(diff.z, diff.x) - M_PI_2;
    clampAngle(yRot);
    *xRot = asinf(diff.y);
}

void calcNPCAi(Ship* playerShip, Ship* npcShip, uint32_t ticks)
{
    float distance = distance3d(&playerShip->position, &npcShip->position);

    if(distance < AI_RANGE)
    {
        float angleY, angleX;
        calcRotToTarget(&npcShip->position, &playerShip->position, &angleY, &angleX);

        if(distance > AI_RANGE_COMBAT_NEAR)
        {
            npcShip->rotation.y = angleY;
            npcShip->rotation.x = angleX;

            accelerateShip(npcShip, 1, ticks);

            //Fire weapons!
            fireWeapons(npcShip, playerShip, 1);
        }
        else if(distance < AI_RANGE_COMBAT_FAR)
        {
            //angleY += 100;
            //npcShip->rotation.y = angleY;
            accelerateShip(npcShip, -1, ticks);
        }
    }
    else
    {
        accelerateShip(npcShip, -1, ticks);
    }
}