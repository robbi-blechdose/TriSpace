#include "spacedust.h"
#include "engine/util.h"
#include <math.h>

//Fields:
//X, Y position
//X, Y direction
float spacedust[NUM_SPACEDUST][4];

#define WINX 240
#define WINY 240
#define WINY_3D (WINY - 70)

void initSpacedust()
{
    for(uint8_t i = 0; i < NUM_SPACEDUST; i++)
    {
        //Initialize to out-of-bounds value so the first calculation run computes proper values
        spacedust[i][0] = -1;
    }
}

void calcSpacedust(Ship* playerShip, uint32_t ticks)
{
    for(uint8_t i = 0; i < NUM_SPACEDUST; i++)
    {
        if(spacedust[i][0] < 0 || spacedust[i][1] < 0 || spacedust[i][0] > 239 || spacedust[i][1] > 239 - 70)
        {
            //Generate angle
            float angle = randf(M_PI * 2);
            spacedust[i][2] = cosf(angle);
            spacedust[i][3] = sinf(angle);
            //Generate position from angle
            spacedust[i][0] = WINX / 2 + spacedust[i][2] * randf(WINX / 2);
            spacedust[i][1] = WINY_3D / 2 + spacedust[i][3] * randf(WINY_3D / 2);
            //Generate speed (which basically represents distance to the player)
            float speed = (0.5f + randf(1.5f)) * 16;
            spacedust[i][2] *= speed;
            spacedust[i][3] *= speed;
        }
        else
        {
            spacedust[i][0] += spacedust[i][2] * playerShip->speed * ticks / 1000.0f;
            spacedust[i][1] += spacedust[i][3] * playerShip->speed * ticks / 1000.0f;
            //TODO: Adjust directions (e.g. a point that was going down should go up if the ship turns downwards)
            if(playerShip->turnSpeedY != 0)
            {
                //- because we have to move the points in the opposite direction
                spacedust[i][0] -= playerShip->turnSpeedY * 128 * ticks / 1000.0f;
            }
            if(playerShip->turnSpeedX != 0)
            {
                //- because we have to move the points in the opposite direction
                spacedust[i][1] -= playerShip->turnSpeedX * 128 * ticks / 1000.0f;
            }
        }
    }
}

void drawSpacedust()
{
    for(uint8_t i = 0; i < NUM_SPACEDUST; i++)
    {
        glPlotPixel((uint8_t) spacedust[i][0], (uint8_t) spacedust[i][1], SPACEDUST_COLOR);
    }
}