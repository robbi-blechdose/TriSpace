#include "spacedust.h"

#include <math.h>

#include "GL/gl.h"
#include "fk-engine-core/util.h"

#define NUM_SPACEDUST_COLORS 3

const GLuint spacedustColors[NUM_SPACEDUST_COLORS] = {
    0xF5EDDF,
    0xDABD8B,
    0xB3FFFF
};

typedef struct {
    float x;
    float y;
    float rotX;
    float rotY;
    uint8_t color;
} Spacedust;

Spacedust spacedust[NUM_SPACEDUST];
Spacedust hyperspaceDust[NUM_HYPERSPACE_DUST][NUM_SPACEDUST];
uint16_t hyperspaceDustIndex = 0;
uint16_t hyperspaceDustMin = 0;
typedef enum {
    HSDM_DISABLED,
    HSDM_INCREASE,
    HSDM_DECREASE,
} HyperspaceDustMode;

HyperspaceDustMode hyperspaceDustMode = HSDM_DISABLED;

#define WINX 240
#define WINY 240
#define WINY_3D (WINY - 70)

void initSpacedust()
{
    for(uint8_t i = 0; i < NUM_SPACEDUST; i++)
    {
        //Initialize to out-of-bounds value so the first calculation run computes proper values
        spacedust[i].x = -1;
    }
}

void calcSpacedust(Ship* playerShip, uint32_t ticks)
{
    if(hyperspaceDustMode == HSDM_INCREASE)
    {
        for(uint8_t i = 0; i < NUM_SPACEDUST; i++)
        {
            hyperspaceDust[hyperspaceDustIndex][i].x = spacedust[i].x;
            hyperspaceDust[hyperspaceDustIndex][i].y = spacedust[i].y;
            hyperspaceDust[hyperspaceDustIndex][i].color = spacedust[i].color;
        }
        hyperspaceDustIndex++;
    }
    else if(hyperspaceDustMode == HSDM_DECREASE)
    {
        hyperspaceDustMin++;
        //Once no dust displays, turn off
        if(hyperspaceDustMin > hyperspaceDustIndex)
        {
            hyperspaceDustMode = HSDM_DISABLED;
        }
    }

    for(uint8_t i = 0; i < NUM_SPACEDUST; i++)
    {
        if(spacedust[i].x < 0 || spacedust[i].y < 0 || spacedust[i].x >= WINX || spacedust[i].y >= WINY_3D)
        {
            //Generate angle
            float angle = randf(M_PI * 2);
            spacedust[i].rotX = cosf(angle);
            spacedust[i].rotY = sinf(angle);
            //Generate position from angle
            spacedust[i].x = WINX / 2 + spacedust[i].rotX * randf(WINX / 2);
            spacedust[i].y = WINY_3D / 2 + spacedust[i].rotY * randf(WINY_3D / 2);
            //Generate speed (which basically represents distance to the player)
            float speed = (0.5f + randf(1.5f)) * 16;
            spacedust[i].rotX *= speed;
            spacedust[i].rotY *= speed;
            //Generate color
            spacedust[i].color = randr(NUM_SPACEDUST_COLORS);
        }
        else
        {
            float speed = playerShip->speed > 20 ? 20 : playerShip->speed;
            spacedust[i].x += spacedust[i].rotX * speed * ticks / 1000.0f;
            spacedust[i].y += spacedust[i].rotY * speed * ticks / 1000.0f;
            //TODO: Adjust directions (e.g. a point that was going down should go up if the ship turns downwards)

            //Move spacedust in the direction opposite the ship's rotation
            if(playerShip->turnSpeedY != 0)
            {
                float angle = playerShip->turnSpeedY * ticks / 1000.0f;

                //Move to origin
                spacedust[i].x -= WINX / 2;
                spacedust[i].y -= WINY_3D / 2;

                //Rotate
                float newX = spacedust[i].x * cosf(angle) - spacedust[i].y * sinf(angle);
                float newY = spacedust[i].x * sinf(angle) + spacedust[i].y * cosf(angle);
                spacedust[i].x = newX;
                spacedust[i].y = newY;

                //Move back
                spacedust[i].x += WINX / 2;
                spacedust[i].y += WINY_3D / 2;
            }
            if(playerShip->turnSpeedX != 0)
            {
                spacedust[i].y += playerShip->turnSpeedX * 128 * ticks / 1000.0f;
            }
        }
    }
}

void drawSpacedust()
{
    if(hyperspaceDustMode != HSDM_DISABLED)
    {
        for(uint8_t i = hyperspaceDustMin; i <= hyperspaceDustIndex; i++)
        {
            for(uint8_t j = 0; j < NUM_SPACEDUST; j++)
            {
                glPlotPixel((uint8_t) hyperspaceDust[i][j].x, (uint8_t) hyperspaceDust[i][j].y, spacedustColors[hyperspaceDust[i][j].color]);
            }
        }
    }

    for(uint8_t i = 0; i < NUM_SPACEDUST; i++)
    {
        glPlotPixel((uint8_t) spacedust[i].x, (uint8_t) spacedust[i].y, spacedustColors[spacedust[i].color]);
    }
}

void enableHyperspaceDust()
{
    hyperspaceDustIndex = 0;
    hyperspaceDustMode = HSDM_INCREASE;
}

void disableHyperspaceDust()
{
    hyperspaceDustMode = HSDM_DECREASE;
}