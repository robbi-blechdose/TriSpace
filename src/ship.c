#include "ship.h"

void calcShip(Ship* ship, uint32_t ticks)
{
    float diff = (ship->speed * ticks) / 1000.0f;
    ship->position.z += cos(ship->rotation.y) * cos(ship->rotation.x) * diff;
    ship->position.x -= sin(ship->rotation.y) * cos(ship->rotation.x) * diff;
    ship->position.y += sin(ship->rotation.x) * diff;
}

void steerShip(Ship* ship, int8_t dirX, int8_t dirY, uint32_t ticks)
{
    ship->rotation.x += (dirX * (float) ticks) / 1000.0f;
    ship->rotation.y += (dirY * (float) ticks) / 1000.0f;
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
}