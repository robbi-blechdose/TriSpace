#include "ship.h"
#include "engine/model.h"
#include "engine/image.h"

GLuint shipMesh;
GLuint shipTexture;

void initShip()
{
    shipMesh = loadModelList("res/obj/Ship.obj");
    shipTexture = loadRGBTexture("res/tex/Ship.png");
}

void drawShip(Ship* ship)
{
    glBindTexture(GL_TEXTURE_2D, shipTexture);
    glPushMatrix();
    glTranslatef(ship->position.x, ship->position.y, ship->position.z);
    //TODO: Rotation
    glCallList(shipMesh);
    glPopMatrix();
}

void calcShip(Ship* ship, StarSystem* starSystem, uint32_t ticks)
{
    ship->rotation.x += (ship->turnSpeedX * ticks) / 1000.0f;
    ship->rotation.y += (ship->turnSpeedY * ticks) / 1000.0f;
    //Keep rotation in bounds
    if(ship->rotation.x < 0)
    {
        ship->rotation.x += 2 * M_PI;
    }
    if(ship->rotation.x > 2 * M_PI)
    {
        ship->rotation.x -= 2 * M_PI;
    }
    if(ship->rotation.y < 0)
    {
        ship->rotation.y += 2 * M_PI;
    }
    if(ship->rotation.y > 2 * M_PI)
    {
        ship->rotation.y -= 2 * M_PI;
    }

    float diff = (ship->speed * ticks) / 1000.0f;
    ship->position.z -= cos(ship->rotation.y) * cos(ship->rotation.x) * diff;
    ship->position.x += sin(ship->rotation.y) * cos(ship->rotation.x) * diff;
    ship->position.y -= sin(ship->rotation.x) * diff;

    //Basic collisions for planets
    for(uint8_t i = 0; i < starSystem->numPlanets; i++)
    {
        if(distance3d(&starSystem->planets[i].position, &ship->position) < starSystem->planets[i].size + 2)
        {
            ship->speed = 0;
            ship->shields = 0;
            //TODO: Move ship away from planets
            break;
        }
    }

    if(ship->shields < ship->type->maxShields)
    {
        ship->shields += ship->type->shieldRegen * ticks / 1000.0f;
        if(ship->shields > ship->type->maxShields)
        {
            ship->shields = ship->type->maxShields;
        }
    }
    if(ship->energy < ship->type->maxEnergy)
    {
        ship->energy += ship->type->energyRegen * ticks / 1000.0f;
        if(ship->energy > ship->type->maxEnergy)
        {
            ship->energy = ship->type->maxEnergy;
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
        if(ship->turnSpeedX > 0)
        {
            ship->turnSpeedX -= ticks / 125.0f;
        }
        else if(ship->turnSpeedX < 0)
        {
            ship->turnSpeedX += ticks / 125.0f;
        }
        if(ship->turnSpeedX < 0.1f && ship->turnSpeedX > -0.1f)
        {
            ship->turnSpeedX = 0;
        }
    }
    if(dirY != 0)
    {
        ship->turnSpeedY += (dirY * (float) ticks) / 125.0f; //8 units per second
    }
    else
    {
        if(ship->turnSpeedY > 0)
        {
            ship->turnSpeedY -= ticks / 125.0f;
        }
        else if(ship->turnSpeedY < 0)
        {
            ship->turnSpeedY += ticks / 125.0f;
        }
        if(ship->turnSpeedY < 0.1f && ship->turnSpeedY > -0.1f)
        {
            ship->turnSpeedY = 0;
        }
    }
    ship->turnSpeedX = clampf(ship->turnSpeedX, -ship->type->maxTurnSpeed, ship->type->maxTurnSpeed);
    ship->turnSpeedY = clampf(ship->turnSpeedY, -ship->type->maxTurnSpeed, ship->type->maxTurnSpeed);
}

void accelerateShip(Ship* ship, int8_t dir, uint32_t ticks)
{
    ship->speed += (dir * (float) ticks) / 125.0f; //8 units per second
    if(ship->speed < 0)
    {
        ship->speed = 0;
    }
    else if(ship->speed > ship->type->maxSpeed)
    {
        ship->speed = ship->type->maxSpeed;
    }
    ship->speed = clampf(ship->speed, 0, ship->type->maxSpeed);
}

void damageShip(Ship* ship, uint8_t damage)
{
    if(damage > ship->shields)
    {
        printf("Destroyed\n");
        //TODO: Mark as destroyed
    }
    else
    {
        ship->shields -= damage;
    }
}

void fireWeapons(Ship* ship, Ship* targetShips, uint8_t numTargets)
{
    if(ship->weapon.timer)
    {
        return;
    }
    if(ship->energy - ship->weapon.type->energyUsage < 0)
    {
        return;
    }

    ship->energy -= ship->weapon.type->energyUsage;
    ship->weapon.timer = ship->weapon.type->cooldown;

    for(uint8_t i = 0; i < numTargets; i++)
    {
        //OC = ray origin to sphere center
        vec3 oc = subv3(ship->position, targetShips[i].position);

        vec3 rot = {.x = 0, .y = 0, .z = 1};
        vec3 dir = {.x = 1, .y = 0, .z = 0};
        rot = rotatev3(rot, dir, ship->rotation.x);
        dir.x = 0;
        dir.y = 1;
        rot = rotatev3(rot, dir, M_PI - ship->rotation.y);
        
        //"Broadphase" hit detection
        float b = dotv3(oc, rot);
        float c = dotv3(oc, oc) - (SHIP_SPHERE_RADIUS * SHIP_SPHERE_RADIUS);
        if(!(c > 0.0f && b > 0.0f))
        {
            float discr = b * b - c;
            if(discr >= 0)
            {
                printf("HIT\n");
                //TODO: Fine detection
                damageShip(&targetShips[i], ship->weapon.type->damage);
                break;
            }
        }
    }
}