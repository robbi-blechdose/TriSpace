#include "ship.h"

void calcShip(Ship* ship, uint32_t ticks)
{
    ship->rotation.x += (ship->turnSpeedX * ticks) / 1000.0f;
    ship->rotation.y += (ship->turnSpeedY * ticks) / 1000.0f;

    float diff = (ship->speed * ticks) / 1000.0f;
    ship->position.z -= cos(ship->rotation.y) * cos(ship->rotation.x) * diff;
    ship->position.x += sin(ship->rotation.y) * cos(ship->rotation.x) * diff;
    ship->position.y -= sin(ship->rotation.x) * diff;
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