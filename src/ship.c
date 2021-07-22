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

void calcShip(Ship* ship, uint32_t ticks)
{
    ship->rotation.x += (ship->turnSpeedX * ticks) / 1000.0f;
    ship->rotation.y += (ship->turnSpeedY * ticks) / 1000.0f;

    float diff = (ship->speed * ticks) / 1000.0f;
    ship->position.z -= cos(ship->rotation.y) * cos(ship->rotation.x) * diff;
    ship->position.x += sin(ship->rotation.y) * cos(ship->rotation.x) * diff;
    ship->position.y -= sin(ship->rotation.x) * diff;

    if(ship->shields < ship->type->maxShields)
    {
        ship->shields += ship->type->shieldRegen;
        if(ship->shields > ship->type->maxShields)
        {
            ship->shields = ship->type->maxShields;
        }
    }
    if(ship->energy < ship->type->maxEnergy)
    {
        ship->energy += ship->type->energyRegen;
        if(ship->energy > ship->type->maxEnergy)
        {
            ship->energy = ship->type->maxEnergy;
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
    ship->turnSpeedX = clamp(ship->turnSpeedX, -ship->type->maxTurnSpeed, ship->type->maxTurnSpeed);
    ship->turnSpeedY = clamp(ship->turnSpeedY, -ship->type->maxTurnSpeed, ship->type->maxTurnSpeed);
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
    ship->speed = clamp(ship->speed, 0, ship->type->maxSpeed);
}

void fireWeapons(Ship* ship)
{
    //TODO
}