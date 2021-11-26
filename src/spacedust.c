#include "spacedust.h"
#include "engine/util.h"

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
        spacedust[i][0] = WINX / 4 + randf(WINX / 2);
        spacedust[i][1] = WINY_3D / 4 + randf(WINY_3D / 2);
    }
}

void calcSpacedust(Ship* playerShip, uint32_t ticks)
{
    for(uint8_t i = 0; i < NUM_SPACEDUST; i++)
    {
        if(spacedust[i][0] < 0 || spacedust[i][1] < 0 || spacedust[i][0] > 239 || spacedust[i][1] > 239 - 70)
        {
            spacedust[i][0] = WINX / 4 + randf(WINX / 2);
            spacedust[i][1] = WINY_3D / 4 + randf(WINY_3D / 2);
            spacedust[i][2] = spacedust[i][0] > (WINX / 2) ? randf(1) : -randf(1);
            spacedust[i][3] = spacedust[i][1] > (WINY_3D / 2) ? randf(1) : -randf(1);
        }
        else
        {
            spacedust[i][0] += spacedust[i][2] * 16 * playerShip->speed * ticks / 1000.0f;
            spacedust[i][1] += spacedust[i][3] * 16 * playerShip->speed * ticks / 1000.0f;
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