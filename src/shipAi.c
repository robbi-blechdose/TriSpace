#include "shipAi.h"
#include <stdlib.h>

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
    float angleY, angleX;
    calcRotToTarget(&npcShip->position, &playerShip->position, &angleY, &angleX);

    switch(npcShip->aiState)
    {
        case STATE_IDLE:
        {
            //Do nothing
            if(npcShip->speed > 0)
            {
                accelerateShip(npcShip, -1, ticks);
            }
            //State transition
            if(distance < AI_RANGE)
            {
                npcShip->aiState = STATE_CIRCLE;
                npcShip->aiRotY = AI_ROT_NONE;
            }
            break;
        }
        case STATE_CIRCLE:
        {
            npcShip->rotation.x = angleX;
            if(distance > AI_RANGE_CIRCLE_INNER)
            {
                if(distance < AI_RANGE_CIRCLE_OUTER)
                {
                    //No direction set? Set one!
                    if(npcShip->aiRotY == AI_ROT_NONE)
                    {
                        if(rand() < RAND_MAX / 2)
                        {
                            npcShip->aiRotY = M_PI_2;
                        }
                        else
                        {
                            npcShip->aiRotY = -M_PI_2;
                        }
                    }
                    //Keep circling
                    npcShip->rotation.y = angleY + npcShip->aiRotY;
                }
                else
                {
                    //Too far out, move closer
                    npcShip->rotation.y = angleY;
                    npcShip->aiRotY = AI_ROT_NONE;
                }
            }
            else
            {
                //Too close, move away
                npcShip->rotation.y = angleY + M_PI;
                npcShip->aiRotY = AI_ROT_NONE;
            }
            //Maintain 3/4 speed
            if(npcShip->speed < shipTypes[npcShip->type].maxSpeed * 0.75f)
            {
                accelerateShip(npcShip, 1, ticks);
            }
            else
            {
                accelerateShip(npcShip, -1, ticks);
            }
            //State transition
            if(distance > AI_RANGE)
            {
                npcShip->aiState = STATE_IDLE;
            }
            else
            {
                if(rand() < RAND_MAX / 4096)
                {
                    npcShip->aiState = STATE_ATTACK;
                    npcShip->aiRotY = AI_ROT_NONE;
                }
            }
            break;
        }
        case STATE_ATTACK:
        {
            if(npcShip->aiRotY == AI_ROT_NONE)
            {
                //Normal attack mode
                npcShip->rotation.y = angleY;
                npcShip->rotation.x = angleX;
                accelerateShip(npcShip, 1, ticks);
                //Fire weapons!
                fireWeapons(npcShip, playerShip, 1);
                if(distance < AI_RANGE_TOONEAR)
                {
                    angleY -= DEG_TO_RAD(35);
                    npcShip->aiRotX = angleX;
                    npcShip->aiRotY = angleY;
                }
            }
            else
            {
                //Veer off
                npcShip->rotation.y = npcShip->aiRotY;
                npcShip->rotation.x = npcShip->aiRotX;
                accelerateShip(npcShip, 1, ticks);
                if(distance > AI_RANGE_TOONEAR)
                {
                    npcShip->aiState = STATE_CIRCLE;
                    npcShip->aiRotY = AI_ROT_NONE;
                }
            }
            break;
        }
    }
}