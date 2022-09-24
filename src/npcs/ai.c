#include "ai.h"

#include "../engine/util.h"

//Friendly: set direction, move at half speed, randomly change direction (small chance)
//Police: random checks for illegal cargo, attack if true, attack if contract ship is damaged by player, attack if shot by player, otherwise patrol
//Enemy: attack when in range

/**
 * UI test code to display npc radar dot:
    //--------------------------------------------------------------------------------------------------
    vec3 diff = subv3(npcs[0].ship.position, player->ship.position);
    quat qr = multQuat(QUAT_INITIAL, inverseQuat(npcs[0].ship.rotation));
    vec3 rot = multQuatVec3(qr, diff);
    rot = normalizev3(rot);
    if(rot.z > 0)
    {
        rot.x = -rot.x * 30;
        rot.y = -rot.y * 30;
    }
    float texY1 = PTC(4 + 0 * 4);
    float texY2 = PTC(7 + 0 * 4);
    drawTexQuad(119.5f + rot.x - 2, 36.5f + rot.y - 2, 4, 4, UITH, PTC(252), texY1, 1, texY2);
    //--------------------------------------------------------------------------------------------------
 **/

void turnTowardsPoint(Npc* npc, vec3 point)
{
    //Project vector from npc to target to 2d (z component is before/behind npc)
    //Also rotate it properly
    vec3 diff = subv3(npc->ship.position, point);
    quat qr = multQuat(QUAT_INITIAL, inverseQuat(npc->ship.rotation));
    vec3 rot = multQuatVec3(qr, diff);
    rot = normalizev3(rot);

    if(rot.z > 0)
    {
        //Turn towards point
        float angle = atan2f(-rot.y, -rot.x);

        rot.x = -rot.x * 30;
        rot.y = -rot.y * 30;

        //Roll
        if(fabsf(rot.x) > 0.05f)
        {
            if(rot.x < 0)
            {
                npc->ship.turnSpeedY = 0.2f;
            }
            else
            {
                npc->ship.turnSpeedY = -0.2f;
            }
        }
        else
        {
            npc->ship.turnSpeedY = 0;
        }

        //Pitch
        /**
         * Restrict pitch to 2 "sections" of the radar circle (to prevent small "staircase" motions when very close to X axis of radar)
         * Section 1: angle > pi/4, angle < 3*pi/4 (upper)
         * Section 2: angle > 5*pi/4, angle < 7*pi/4 (lower)
         **/
        if(((angle > M_PI_4 && angle < 3 * M_PI_4) || (angle > 5 * M_PI_4 && angle < 7 * M_PI_4)) &&
            fabsf(rot.y) > 0.05f )
        {
            if(rot.y < 0)
            {
                npc->ship.turnSpeedX = -0.2f;
            }
            else
            {
                npc->ship.turnSpeedX = 0.2f;
            }
        }
        else
        {
            npc->ship.turnSpeedX = 0;
        }
    }
    else
    {
        //Turn until the point is in front of the npc
        float angle = atan2f(-rot.y, -rot.x);
        rot.x = 32 * cosf(angle);
        rot.y = 32 * sinf(angle);

        if(rot.y < 0)
        {
            npc->ship.turnSpeedX = -0.5f;
        }
        else
        {
            npc->ship.turnSpeedX = 0.5f;
        }
    }
}

void calcNPCAiEnemy(Npc* npc, Player* player, uint32_t ticks, float distanceToPlayer)
{
    if(distanceToPlayer > AI_RANGE)
    {
        return;
    }
    
    turnTowardsPoint(npc, player->ship.position);

    //TODO
}

void calcNPCAi(Npc* npc, Player* player, Npc* npcs, uint32_t ticks)
{
    float distanceToPlayer = distance3(player->ship.position, npc->ship.position);

    switch(npc->ship.type)
    {
        case SHIP_TYPE_SMALLPIRATE:
        {
            calcNPCAiEnemy(npc, player, ticks, distanceToPlayer);
            break;
        }
        case SHIP_TYPE_CRUISELINER:
        {
            break;
        }
        case SHIP_TYPE_POLICE:
        {
            break;
        }
    }
}