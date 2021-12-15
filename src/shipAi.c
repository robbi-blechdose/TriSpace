#include "shipAi.h"
#include <stdlib.h>

void calcRotToTarget(vec3* pos, vec3* target, float* yRot, float* xRot)
{
    vec3 diff = subv3(*pos, *target);
    diff = normalizev3(diff);

    *yRot = atan2f(diff.z, diff.x) - M_PI_2;
    clampAngle(yRot);
    *xRot = asinf(diff.y); //TODO: Improve x rot
}

void calcNPCAiEnemy(Ship* playerShip, Ship* npcShip, uint32_t ticks, float distance, float angleX, float angleY, float* targetX, float* targetY)
{
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
                    *targetY = angleY + npcShip->aiRotY;
                }
                else
                {
                    //Too far out, move closer
                    *targetY = angleY;
                    npcShip->aiRotY = AI_ROT_NONE;
                }
            }
            else
            {
                //Too close, move away
                *targetY = (angleY + M_PI);
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
                if(rand() < RAND_MAX / 2048)
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
                *targetY = angleY;
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
                *targetY = npcShip->aiRotY;
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

void calcNPCAiPolice(Ship* playerShip, Ship* npcShip, uint32_t ticks, float distance, float angleX, float angleY, float* targetX, float* targetY)
{
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
            if(distance < AI_RANGE * 5)
            {
                if(rand() < RAND_MAX / 2048)
                {
                    npcShip->aiState = STATE_POLICE_CHECK;
                }
            }
            break;
        }
        case STATE_POLICE_CHECK:
        {
            npcShip->aiState = STATE_IDLE;
            for(uint8_t i = 0; i < playerShip->hold.size; i++)
            {
                if(playerShip->hold.cargo[i] > 0 && isCargoIllegal(i))
                {
                    //Found illegal cargo
                    npcShip->aiState = STATE_ATTACK;
                }
            }
            break;
        }
        //TODO: Attack state
    }
}

void calcNPCAiFriendly(Ship* npcShip, uint32_t ticks, float distance, float angleX, float angleY, float* targetX, float* targetY)
{
    accelerateShip(npcShip, 1, ticks);
    *targetY = 0;
    //TODO
}

void calcNPCAi(Ship* playerShip, Ship* npcShip, uint32_t ticks)
{
    float distance = distance3d(&playerShip->position, &npcShip->position);
    float angleX, angleY;
    calcRotToTarget(&npcShip->position, &playerShip->position, &angleY, &angleX);
    float targetX, targetY;

    switch(npcShip->type)
    {
        case SHIP_TYPE_SMALLPIRATE:
        {
            calcNPCAiEnemy(playerShip, npcShip, ticks, distance, angleX, angleY, &targetX, &targetY);
            break;
        }
        case SHIP_TYPE_CRUISELINER:
        {
            calcNPCAiFriendly(npcShip, ticks, distance, angleX, angleY, &targetX, &targetY);
            break;
        }
        case SHIP_TYPE_POLICE:
        {
            calcNPCAiPolice(playerShip, npcShip, ticks, distance, angleX, angleY, &targetX, &targetY);
            break;
        }
    }

    clampAngle(&targetY);
    float turnY = 0;
    if(npcShip->rotation.y < targetY)
    {
        if(fabs(npcShip->rotation.y - targetY) < M_PI)
        {
            turnY = shipTypes[npcShip->type].maxTurnSpeed; //npcShip->rotation.y - targetY;
        }
        else
        {
            turnY = -shipTypes[npcShip->type].maxTurnSpeed; //(npcShip->rotation.y - targetY);
        }
    }
    else
    {
        if(fabs(npcShip->rotation.y - targetY) < M_PI)
        {
            turnY = -shipTypes[npcShip->type].maxTurnSpeed; //-(npcShip->rotation.y - targetY);
        }
        else
        {
            turnY = shipTypes[npcShip->type].maxTurnSpeed; //npcShip->rotation.y - targetY;
        }
    }
    //printf("dist: %f rot: %f target: %f diff: %f\n", distance, npcShip->rotation.y, targetY, turnY);
    npcShip->turnSpeedY = clampf(turnY, -shipTypes[npcShip->type].maxTurnSpeed, shipTypes[npcShip->type].maxTurnSpeed);
}