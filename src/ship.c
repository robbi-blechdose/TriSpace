#include "ship.h"
#include "engine/model.h"
#include "engine/image.h"
#include "engine/effects.h"
#include "engine/audio.h"
#include "universe/asteroids.h"

GLuint shipMeshes[NUM_SHIP_TYPES];
GLuint shipTextures[NUM_SHIP_TYPES];

const ShipType shipTypes[NUM_SHIP_TYPES] = {
    //Normal enemy ship
    {.maxSpeed = 10, .maxTurnSpeed = 5, .maxShields = 5, .maxEnergy = 5, .shieldRegen = 1, .energyRegen = 1, .hitSphere = 1.5f},
    //Cruise liner ship
    {.maxSpeed = 5, .maxTurnSpeed = 2, .maxShields = 10, .maxEnergy = 5, .shieldRegen = 1, .energyRegen = 1, .hitSphere = 5.0f},
    //Police ship
    {.maxSpeed = 10, .maxTurnSpeed = 5, .maxShields = 6, .maxEnergy = 6, .shieldRegen = 1, .energyRegen = 1, .hitSphere = 1.5f}
};

const WeaponType weaponTypes[] = {
    {.cooldown = 400, .damage = 2, .energyUsage = 1, .mineChance = 25}, //MkI laser
    {.cooldown = 350, .damage = 2, .energyUsage = 1, .mineChance = 20}, //MkII laser
    {.cooldown = 300, .damage = 4, .energyUsage = 2, .mineChance = 20}, //MkIII military laser
    {.cooldown = 450, .damage = 2, .energyUsage = 3, .mineChance = 75}  //Mining laser
};

uint8_t sampleShoot;

void initShip()
{
    shipMeshes[0] = loadModelList("res/obj/Ship.obj");
    shipTextures[0] = loadRGBTexture("res/tex/PirateShip.png");
    shipMeshes[1] = loadModelList("res/obj/CruiseShip.obj");
    shipTextures[1] = loadRGBTexture("res/tex/CruiseShip.png");
    shipMeshes[2] = loadModelList("res/obj/PoliceShip.obj");
    shipTextures[2] = loadRGBTexture("res/tex/PoliceShip.png");
    sampleShoot = loadSample("res/sfx/flaunch.wav");
}

void drawShip(Ship* ship)
{
    glBindTexture(GL_TEXTURE_2D, shipTextures[ship->type]);
    glPushMatrix();
    glTranslatef(ship->position.x, ship->position.y, ship->position.z);
    glRotatef(RAD_TO_DEG(M_PI - ship->rotation.y), 0, 1, 0);
    glRotatef(RAD_TO_DEG(ship->rotation.x), 1, 0, 0);
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

void calcShip(Ship* ship, uint8_t collided, uint32_t ticks)
{
    ship->rotation.x += (ship->turnSpeedX * ticks) / 1000.0f;
    ship->rotation.y += (ship->turnSpeedY * ticks) / 1000.0f;
    //Keep rotation in bounds
    clampAngle(&ship->rotation.x);
    clampAngle(&ship->rotation.y);

    float diff = (ship->speed * ticks) / 1000.0f;
    ship->position.z -= cos(ship->rotation.y) * cos(ship->rotation.x) * diff;
    ship->position.x += sin(ship->rotation.y) * cos(ship->rotation.x) * diff;
    ship->position.y -= sin(ship->rotation.x) * diff;

    if(collided)
    {
        ship->speed = 0;
        ship->shields = 0;
        //TODO: Move ship away
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

void accelerateShip(Ship* ship, int8_t dir, uint32_t ticks)
{
    ship->speed += (dir * (float) ticks) / 125.0f; //8 units per second
    ship->speed = clampf(ship->speed, 0, shipTypes[ship->type].maxSpeed);
}

uint8_t damageShip(Ship* ship, uint8_t damage)
{
    ship->damaged = 1;
    ship->shields -= damage;
    return ship->shields < 0;
}

uint8_t shipIsDestroyed(Ship* ship)
{
    return ship->shields < 0;
}

void fireWeapons(Ship* ship, Ship* targetShips, uint8_t numTargets)
{
    if(ship->weapon.timer)
    {
        return;
    }
    if(ship->energy < weaponTypes[ship->weapon.type].energyUsage)
    {
        return;
    }

    ship->energy -= weaponTypes[ship->weapon.type].energyUsage;
    ship->weapon.timer = weaponTypes[ship->weapon.type].cooldown;

    playSample(sampleShoot);

    //Calculate ray direction vector
    vec3 dir = anglesToDirection(&ship->rotation);

    //Check ship hits
    for(uint8_t i = 0; i < numTargets; i++)
    {
        if(targetShips[i].type == SHIP_TYPE_NULL)
        {
            continue;
        }

        float hit = checkHitSphere(&ship->position, &dir, &targetShips[i].position, shipTypes[targetShips[i].type].hitSphere);
        if(hit != -1)
        {
            ship->weapon.distanceToHit = hit;
            uint8_t destroyed = damageShip(&targetShips[i], weaponTypes[ship->weapon.type].damage);
            if(!destroyed)
            {
                createEffect(targetShips[i].position, SPARKS);
            }
            return;
        }
    }

    //Check asteroid hits
    checkAsteroidHit(&ship->position, &dir, weaponTypes[ship->weapon.type].damage, weaponTypes[ship->weapon.type].mineChance);
}

float getTurnSpeedForRotation(float current, float target, float maxSpeed)
{
    clampAngle(&target);
    if(fabs(current - target) > 0.05f)
    {
        float turnY = 0;
        if(current < target)
        {
            if(fabs(current - target) < M_PI)
            {
                return maxSpeed;
            }
            else
            {
                return -maxSpeed;
            }
        }
        else
        {
            if(fabs(current - target) < M_PI)
            {
                return -maxSpeed;
            }
            else
            {
                return maxSpeed;
            }
        }
    }
    else
    {
        return 0;
    }
}