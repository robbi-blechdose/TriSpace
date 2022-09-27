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

/**
 * Turns the NPC towards the point by setting the turn speed values
 * Internally uses a radar similar to the player's
 * Returns the distance of the target dot to the center of the radar (how close the npc is to looking at the point)
 **/
float turnTowardsPoint(Npc* npc, vec3 point)
{
    //Project vector from npc to target to 2d (z component is before/behind npc)
    //Also rotate it properly
    vec3 diff = subv3(npc->ship.position, point);
    quat qr = multQuat(QUAT_INITIAL, inverseQuat(npc->ship.rotation));
    vec3 rot = multQuatVec3(qr, diff);
    rot = normalizev3(rot);
    //printf("%f | %f\n", rot.y, dotv3(normalizev3(diff), multQuatVec3(npc->ship.rotation, (vec3) {0, 1, 0})));

    if(rot.z > 0)
    {
        //Turn towards point

        float angle = atan2f(rot.y, rot.x) + M_PI_2;
        if(angle > M_PI)
        {
            angle -= M_PI;
        }

        //If the "radar dot" is below the y axis, invert roll direction (roll to lower half of radar)
        if(angle < -M_PI_2 || angle > M_PI_2)
        {
            angle -= M_PI;
            if(angle < -M_PI)
            {
                angle += M_PI;
            }
        }

        //Roll
        if(fabsf(rot.x) > 0.0001f)
        {
            npc->ship.turnSpeedY = shipTypes[npc->ship.type].maxTurnSpeed * (angle / M_PI);
        }
        else
        {
            npc->ship.turnSpeedY = 0;
        }

        //Pitch
        if(fabsf(rot.y) > 0.0001f)
        {
            //Scale pitch by roll (reduce pitch when roll is high)
            float pitch = (-rot.y) * (1 - sqrtf(fabsf(angle / M_PI)));
            npc->ship.turnSpeedX = shipTypes[npc->ship.type].maxTurnSpeed * pitch;
        }
        else
        {
            npc->ship.turnSpeedX = 0;
        }

        return sqrtf(squaref(rot.x) + squaref(rot.y));
    }
    else
    {
        //Turn until the point is in front of the npc
        float angle = atan2f(-rot.y, -rot.x);

        if(sinf(angle) < 0)
        {
            npc->ship.turnSpeedX = -shipTypes[npc->ship.type].maxTurnSpeed * 0.7f;
        }
        else
        {
            npc->ship.turnSpeedX = shipTypes[npc->ship.type].maxTurnSpeed * 0.7f;
        }

        return 10; //We have no good value to return here, so return a large one since we're facing the wrong way
    }
}

vec3 getRandomSpherePoint(vec3 center, float radius)
{
    vec3 vec = {.x = randf(1), .y = randf(1), .z = randf(1)};
    vec = normalizev3(vec);
    vec = scalev3(radius, vec);
    return vec;
}

void calcNPCAiStateCircle(Npc* npc, Player* player, uint32_t ticks, float distanceToPlayer)
{
    //Move towards waypoint
    turnTowardsPoint(npc, npc->waypoint);
    accelerateShipLimit(&npc->ship, 1, ticks, 0.5f);

    if(distance3d(&npc->waypoint, &npc->ship.position) < AI_RANGE_WAYPOINT)
    {
        //Calculate next waypoint
        npc->waypoint = getRandomSpherePoint(player->ship.position, AI_RANGE_CIRCLE);
    }

    //Randomly attack
    if(rand() < RAND_MAX / 2048)
    {
        npc->state = STATE_ATTACK;
    }
}

void calcNPCAiStateAttack(Npc* npc, Player* player, uint32_t ticks, float distanceToPlayer)
{
    float diff = turnTowardsPoint(npc, player->ship.position);
    accelerateShip(&npc->ship, 1, ticks);

    if(diff < 0.3f)
    {
        //Looking at the player closely enough to fire
        if(fireWeapons(&npc->ship))
        {
            Ship* temp[1] = {&player->ship};
            checkWeaponsShipHit(&npc->ship, temp, 1, DAMAGE_SOURCE_NPC);
        }
    }

    if(distanceToPlayer < AI_RANGE_VEEROFF)
    {
        npc->waypoint = addv3(player->ship.position, (vec3) {0, randr(10) < 5 ? 20 : -20, 0});
        npc->state = STATE_CIRCLE;
    }
}

void calcNPCAiEnemy(Npc* npc, Player* player, uint32_t ticks, float distanceToPlayer)
{
    if(distanceToPlayer > AI_RANGE)
    {
        //Out of range, do nothing
        npc->state = STATE_IDLE;
        accelerateShip(&npc->ship, -1, ticks);
        return;
    }

    switch(npc->state)
    {
        case STATE_IDLE:
        {
            if(distanceToPlayer > AI_RANGE_CIRCLE)
            {
                //Move toward player to get into range
                turnTowardsPoint(npc, player->ship.position);
                accelerateShipLimit(&npc->ship, 1, ticks, 0.5f);
            }
            else
            {
                npc->waypoint = npc->ship.position;
                npc->state = STATE_CIRCLE;
            }
            break;
        }
        case STATE_CIRCLE:
        {
            calcNPCAiStateCircle(npc, player, ticks, distanceToPlayer);
    
            //Flee if being chased by player
            if(distanceToPlayer < AI_RANGE_FLEE)
            {
                npc->waypoint = npc->ship.position;
                npc->state = STATE_FLEE;
            }
            break;
        }
        case STATE_ATTACK:
        {
            calcNPCAiStateAttack(npc, player, ticks, distanceToPlayer);
            break;
        }
        case STATE_FLEE:
        {
            if(distance3d(&npc->waypoint, &npc->ship.position) < AI_RANGE_WAYPOINT)
            {
                vec3 toPlayer = addv3(npc->ship.position, subv3(npc->ship.position, player->ship.position));
                npc->waypoint = toPlayer;
            }

            turnTowardsPoint(npc, npc->waypoint);
            accelerateShip(&npc->ship, 1, ticks);

            //Return to circle
            if(distanceToPlayer > AI_RANGE_FLEE)
            {
                npc->waypoint = npc->ship.position;
                npc->state = STATE_CIRCLE;
            }

            break;
        }
    }
}

void calcNPCAiPolice(Npc* npc, Player* player, uint32_t ticks, float distanceToPlayer)
{
    //Do police check
    if(distanceToPlayer < AI_RANGE_POLICECHECK && rand() < RAND_MAX / 4096)
    {
        for(uint8_t i = 0; i < player->hold.size; i++)
        {
            if(player->hold.cargo[i] > 0 && isCargoIllegal(i))
            {
                //Found illegal cargo, attack
                npc->state = STATE_CIRCLE;
                break;
            }
        }
    }

    if(npc->ship.damaged == DAMAGE_SOURCE_PLAYER || player->wantedLevel >= WANTED_LEVEL_DANGEROUS)
    {
        //Damaged by player or wanted level high enough, attack
        npc->ship.damaged = 0;
        npc->state = STATE_CIRCLE;
    }

    //TODO: check contract ship for damage?

    switch(npc->state)
    {
        case STATE_IDLE:
        {
            if(distanceToPlayer > AI_RANGE)
            {
                //Out of range, do nothing
                accelerateShip(&npc->ship, -1, ticks);
            }
            break;
        }
        case STATE_CIRCLE:
        {
            calcNPCAiStateCircle(npc, player, ticks, distanceToPlayer);
            break;
        }
        case STATE_ATTACK:
        {
            calcNPCAiStateAttack(npc, player, ticks, distanceToPlayer);
            break;
        }
    }

    //Increase player wanted level when destroyed
    if(shipIsDestroyed(&npc->ship))
    {
        if(player->wantedLevel < MAX_WANTED_LEVEL)
        {
            player->wantedLevel++;
        }
    }
}

void calcNPCAiFriendly(Npc* npc, uint32_t ticks)
{
    //TODO
}

void calcNPCAi(Npc* npc, Player* player, Npc* npcs, uint32_t ticks)
{
    float distanceToPlayer = distance3(player->ship.position, npc->ship.position);

    switch(npc->ship.type)
    {
        case SHIP_TYPE_SMALLPIRATE:
        case SHIP_TYPE_SMALLPIRATE2:
        {
            calcNPCAiEnemy(npc, player, ticks, distanceToPlayer);
            break;
        }
        case SHIP_TYPE_CRUISELINER:
        {
            calcNPCAiFriendly(npc, ticks);
            break;
        }
        case SHIP_TYPE_POLICE:
        {
            calcNPCAiPolice(npc, player, ticks, distanceToPlayer);
            break;
        }
    }
}