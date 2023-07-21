#include "ship.h"
#include "engine/model.h"
#include "engine/image.h"
#include "engine/effects.h"
#include "engine/audio.h"
#include "universe/asteroids.h"

GLuint shipMeshes[NUM_SHIP_TYPES];
GLuint shipTextures[NUM_SHIP_TYPES];

const ShipTypeData shipTypes[NUM_SHIP_TYPES] = {
    [SHIP_TYPE_PLAYER] = {.maxSpeed = 10, .maxTurnSpeed = 5, .maxShields = 5, .maxEnergy = 5, .shieldRegen = 0.5f, .energyRegen = 1, .hitSphere = 1.5f},
    [SHIP_TYPE_SMALLPIRATE] = {.maxSpeed = 10, .maxTurnSpeed = 2.5f, .maxShields = 5, .maxEnergy = 5, .shieldRegen = 0.5f, .energyRegen = 1, .hitSphere = 1.5f},
    [SHIP_TYPE_SMALLPIRATE2] = {.maxSpeed = 11, .maxTurnSpeed = 2, .maxShields = 6, .maxEnergy = 5, .shieldRegen = 0.5f, .energyRegen = 1, .hitSphere = 1.5f},
    [SHIP_TYPE_CRUISELINER] = {.maxSpeed = 5, .maxTurnSpeed = 1, .maxShields = 10, .maxEnergy = 5, .shieldRegen = 0.5f, .energyRegen = 1, .hitSphere = 5.0f},
    [SHIP_TYPE_POLICE] = {.maxSpeed = 10, .maxTurnSpeed = 3, .maxShields = 6, .maxEnergy = 6, .shieldRegen = 0.5f, .energyRegen = 1, .hitSphere = 1.5f},
    [SHIP_TYPE_ALIEN] = {.maxSpeed = 11, .maxTurnSpeed = 4, .maxShields = 7, .maxEnergy = 7, .shieldRegen = 0.3f, .energyRegen = 1.1f, .hitSphere = 1.5f},
    [SHIP_TYPE_SPHERE_PIRATE] = {.maxSpeed = 9, .maxTurnSpeed = 2, .maxShields = 8, .maxEnergy = 5, .shieldRegen = 0.4f, .energyRegen = 1, .hitSphere = 1.5f}
};

const char* shipAssets[NUM_SHIP_TYPES][2] = {
    [SHIP_TYPE_PLAYER] = {0, 0},
    [SHIP_TYPE_SMALLPIRATE] = {"res/obj/ships/Ship.obj", "res/tex/ships/PirateShip.png"},
    [SHIP_TYPE_SMALLPIRATE2] = {"res/obj/ships/Ship_02.obj", "res/tex/ships/Ship_02.png"},
    [SHIP_TYPE_CRUISELINER] = {"res/obj/ships/CruiseShip.obj", "res/tex/ships/CruiseShip.png"},
    [SHIP_TYPE_POLICE] = {"res/obj/ships/PoliceShip.obj", "res/tex/ships/PoliceShip.png"},
    [SHIP_TYPE_ALIEN] = {"res/obj/ships/AlienShip.obj", "res/tex/ships/AlienShip.png"},
    [SHIP_TYPE_SPHERE_PIRATE] = {"res/obj/ships/SphereShip.obj", "res/tex/ships/SphereShip.png"}
};

const WeaponType weaponTypes[] = {
    {.cooldown = 400, .damage = 2, .energyUsage = 1, .mineChance = 15}, //MkI laser
    {.cooldown = 350, .damage = 3, .energyUsage = 1, .mineChance = 8},  //MkII laser
    {.cooldown = 300, .damage = 4, .energyUsage = 2, .mineChance = 8},  //MkIII military laser
    {.cooldown = 400, .damage = 2, .energyUsage = 3, .mineChance = 40}  //Mining laser
};

uint8_t sampleShoot;

void initShip()
{
    for(uint8_t i = 1; i < NUM_SHIP_TYPES; i++)
    {
        shipMeshes[i] = loadModelList(shipAssets[i][0]);
        shipTextures[i] = loadRGBTexture(shipAssets[i][1]);
    }
    //The player ship is only used for the contract UI and otherwise never drawn,
    //so we can just steal the mesh from the first small pirate ship
    shipMeshes[0] = shipMeshes[1];
    
    sampleShoot = loadSample("res/sfx/flaunch.wav");
}

void quitShip()
{
    //Start at 1 to skip player ship
    for(uint8_t i = 1; i < NUM_SHIP_TYPES; i++)
    {
        glDeleteList(shipMeshes[i]);
        deleteRGBTexture(shipTextures[i]);
    }
}

void drawShip(Ship* ship)
{
    glBindTexture(GL_TEXTURE_2D, shipTextures[ship->type]);
    glPushMatrix();
    glTranslatef(ship->position.x, ship->position.y, ship->position.z);
    
    float rot[16];
    quat temp = multQuat(quatFromAngles((vec3) {0, M_PI, 0}), inverseQuat(ship->rotation));
    quatToMatrix(rot, temp);
    glMultMatrixf(rot);

    glCallList(shipMeshes[ship->type]);
    if(ship->weapon.timer > (weaponTypes[ship->weapon.type].cooldown / 2))
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glColor3f(0, 0.9f, 1.0f);
        glBegin(GL_LINES);
        glVertex3f(0.5f, 0, 0);
        glVertex3f(0, 0.5f, ship->weapon.distanceToHit - 2.5f);
        glVertex3f(-0.5f, 0, 0);
        glVertex3f(0, 0.5f, ship->weapon.distanceToHit - 2.5f);
        glEnd();
        glColor3f(1, 1, 1);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    glPopMatrix();
}

void calcShip(Ship* ship, uint32_t ticks)
{
    //Update rotation
    float pitch = (ship->turnSpeedX * ticks) / 1000.0f;
    float roll = (ship->turnSpeedY * ticks) / 1000.0f;
    quat temp = quatFromAngles((vec3) {pitch, 0, roll});
    ship->rotation = multQuat(ship->rotation, temp);

    //Update position
    float diff = (ship->speed * ticks) / 1000.0f;
    if(diff > 0)
    {
        vec3 posDiff = multQuatVec3(ship->rotation, (vec3) {0, 0, -1});
        posDiff = scalev3(diff, posDiff);
        ship->position = addv3(ship->position, posDiff);
    }

    if(ship->shields < shipTypes[ship->type].maxShields)
    {
        ship->shields += shipTypes[ship->type].shieldRegen * ticks / 1000.0f;
        if(ship->shields > shipTypes[ship->type].maxShields)
        {
            ship->shields = shipTypes[ship->type].maxShields;
        }
    }
    if(ship->energy < shipTypes[ship->type].maxEnergy)
    {
        ship->energy += shipTypes[ship->type].energyRegen * ticks / 1000.0f;
        if(ship->energy > shipTypes[ship->type].maxEnergy)
        {
            ship->energy = shipTypes[ship->type].maxEnergy;
        }
    }

    if(ship->weapon.timer)
    {
        ship->weapon.timer -= ticks;
        if(ship->weapon.timer < 0)
        {
            ship->weapon.timer = 0;
        }
    }
}

void steerShip(Ship* ship, int8_t dirX, int8_t dirY, uint32_t ticks)
{
    if(dirX != 0)
    {
        ship->turnSpeedX += (dirX * (float) ticks) / 125.0f; //8 units per second
    }
    else
    {
        if(ship->turnSpeedX < 0.1f && ship->turnSpeedX > -0.1f)
        {
            ship->turnSpeedX = 0;
        }
        else if(ship->turnSpeedX > 0)
        {
            ship->turnSpeedX -= ticks / 125.0f;
        }
        else if(ship->turnSpeedX < 0)
        {
            ship->turnSpeedX += ticks / 125.0f;
        }
    }
    if(dirY != 0)
    {
        ship->turnSpeedY += (dirY * (float) ticks) / 125.0f; //8 units per second
    }
    else
    {
        if(ship->turnSpeedY < 0.1f && ship->turnSpeedY > -0.1f)
        {
            ship->turnSpeedY = 0;
        }
        else if(ship->turnSpeedY > 0)
        {
            ship->turnSpeedY -= ticks / 125.0f;
        }
        else if(ship->turnSpeedY < 0)
        {
            ship->turnSpeedY += ticks / 125.0f;
        }
    }
    ship->turnSpeedX = clampf(ship->turnSpeedX, -shipTypes[ship->type].maxTurnSpeed, shipTypes[ship->type].maxTurnSpeed);
    ship->turnSpeedY = clampf(ship->turnSpeedY, -shipTypes[ship->type].maxTurnSpeed, shipTypes[ship->type].maxTurnSpeed);
}

float turnShipTowardsPoint(Ship* ship, vec3 point)
{
    //Project vector from ship to target to 2d (z component is before/behind npc)
    //Also rotate it properly
    vec3 diff = subv3(ship->position, point);
    quat qr = multQuat(QUAT_INITIAL, inverseQuat(ship->rotation));
    vec3 rot = multQuatVec3(qr, diff);
    rot = normalizev3(rot);

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
            ship->turnSpeedY = shipTypes[ship->type].maxTurnSpeed * (angle / M_PI);
        }
        else
        {
            ship->turnSpeedY = 0;
        }

        //Pitch
        if(fabsf(rot.y) > 0.0001f)
        {
            //Scale pitch by roll (reduce pitch when roll is high)
            float pitch = (-rot.y) * (1 - sqrtf(fabsf(angle / M_PI)));
            ship->turnSpeedX = shipTypes[ship->type].maxTurnSpeed * pitch;
        }
        else
        {
            ship->turnSpeedX = 0;
        }

        return sqrtf(squaref(rot.x) + squaref(rot.y));
    }
    else
    {
        //Turn until the point is in front of the ship
        float angle = atan2f(-rot.y, -rot.x);

        if(sinf(angle) < 0)
        {
            ship->turnSpeedX = -shipTypes[ship->type].maxTurnSpeed * 0.7f;
        }
        else
        {
            ship->turnSpeedX = shipTypes[ship->type].maxTurnSpeed * 0.7f;
        }

        return 10; //We have no good value to return here, so return a large one since we're facing the wrong way
    }
}

void accelerateShipLimit(Ship* ship, int8_t dir, uint32_t ticks, float max)
{
    ship->speed += (dir * (float) ticks) / 125.0f; //8 units per second
    ship->speed = clampf(ship->speed, 0, shipTypes[ship->type].maxSpeed * max);
}

void accelerateShip(Ship* ship, int8_t dir, uint32_t ticks)
{
    accelerateShipLimit(ship, dir, ticks, 1.0f);
}

bool damageShip(Ship* ship, uint8_t damage, uint8_t source)
{
    ship->damaged = source;
    ship->shields -= damage;
    return ship->shields < 0;
}

bool shipIsDestroyed(Ship* ship)
{
    return ship->shields < 0;
}

bool fireWeapons(Ship* ship)
{
    if(ship->weapon.timer)
    {
        return false;
    }
    if(ship->energy < weaponTypes[ship->weapon.type].energyUsage)
    {
        return false;
    }

    ship->energy -= weaponTypes[ship->weapon.type].energyUsage;
    ship->weapon.timer = weaponTypes[ship->weapon.type].cooldown;

    playSample(sampleShoot);
    return true;
}

float checkRayShipHit(Ship* ship, Ship* targetShips[], uint8_t numTargets, Ship** hitShip)
{
    //Calculate ray direction vector
    vec3 dir = multQuatVec3(ship->rotation, (vec3) {0, 0, -1});

    //Check ship hits
    for(uint8_t i = 0; i < numTargets; i++)
    {
        if(targetShips[i]->type == SHIP_TYPE_NULL)
        {
            continue;
        }

        float hit = checkHitSphere(&ship->position, &dir, &targetShips[i]->position, shipTypes[targetShips[i]->type].hitSphere);
        if(hit != -1)
        {
            *hitShip = targetShips[i];
            return hit;
        }
    }

    return -1;
}

bool checkWeaponsShipHit(Ship* ship, Ship* targetShips[], uint8_t numTargets, uint8_t source)
{
    Ship* hitShip;
    float distance = checkRayShipHit(ship, targetShips, numTargets, &hitShip);
    if(distance != -1)
    {
        ship->weapon.distanceToHit = distance;
        bool destroyed = damageShip(hitShip, weaponTypes[ship->weapon.type].damage, source);
        if(!destroyed)
        {
            createEffect(hitShip->position, SPARKS);
        }
        return true;
    }

    return false;
}