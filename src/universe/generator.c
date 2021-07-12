#include "generator.h"
#include <stdlib.h>
#include "GL/gl.h"
#define FNL_IMPL
#include "FastNoiseLite.h"

#define NUM_PALETTES 2
Color palettes[NUM_PALETTES][8] = {
    //Earth-type planet
    {
        //Ocean
        {.r = 33, .g = 13, .b = 130},
        {.r = 32, .g = 41, .b = 212},
        {.r = 122, .g = 228, .b = 240},
        //Sand
        {.r = 255, .g = 239, .b = 145},
        {.r = 250, .g = 226, .b = 92},
        //Grass
        {.r = 73, .g = 163, .b = 42},
        {.r = 105, .g = 212, .b = 68},
        {.r = 41, .g = 41, .b = 41}
    },
    //Mars-type planet
    {
        {.r = 145, .g = 69, .b = 25},
        {.r = 199, .g = 88, .b = 24},
        {.r = 227, .g = 132, .b = 36},
        {.r = 240, .g = 153, .b = 67},
        {.r = 247, .g = 163, .b = 89},
        {.r = 199, .g = 160, .b = 125},
        {.r = 199, .g = 145, .b = 97},
        {.r = 255, .g = 255, .b = 255}
    }
};

Color getColorForValue(Color* palette, float value)
{
    uint8_t index = value / 32;
    Color a = palette[index];
    Color b = palette[index + 1];
    Color ret;
    ret.r = a.r + (b.r - a.r) * ((value / 32) - index);
    ret.g = a.g + (b.g - a.g) * ((value / 32) - index);
    ret.b = a.b + (b.b - a.b) * ((value / 32) - index);
    return ret;
}

uint32_t randr(uint32_t max)
{
    return rand() / (RAND_MAX / max + 1);
}

float randf(float max)
{
    return ((float)rand() / (float)(RAND_MAX)) * max;
}

GLuint generatePlanetTexture(uint32_t seed)
{
    uint8_t data[256 * 256 * 3];
    uint16_t i, j;
    uint8_t paletteIndex = randr(2);

    //Generate texture
    fnl_state noise = fnlCreateState();
    noise.noise_type = FNL_NOISE_VALUE;
    noise.fractal_type = FNL_FRACTAL_FBM;
    noise.seed = seed + rand();
    for(i = 0; i < 256; i++)
    {
        for(j = 0; j < 256; j++)
        {
            float temp = (fnlGetNoise2D(&noise, i * 4, j * 4) + 1.0f) * 128;
            Color c = getColorForValue(palettes[paletteIndex], temp);
            data[i * 256 * 3 + j * 3] = c.r;
            data[i * 256 * 3 + j * 3 + 1] = c.g;
            data[i * 256 * 3 + j * 3 + 2] = c.b;
        }
    }

	GLuint t = 0;
	glGenTextures(1, &t);
	glBindTexture(GL_TEXTURE_2D, t);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, 256, 256, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	return t;
}

void generateStarSystem(StarSystem* system, uint32_t seed)
{
    srand(seed);

    system->numStars = randr(4);
    system->numPlanets = randr(9);

    float baseStarSize = 30 + randf(50);

    uint8_t i;
    for(i = 0; i < system->numStars; i++)
    {
        system->stars[i].size = baseStarSize + randf(5);
        system->stars[i].position.x = 0;
        system->stars[i].position.y = 0;
        system->stars[i].position.z = 0;
    }

    for(i = 0; i < system->numPlanets; i++)
    {
        system->planets[i].size = 10.0f + randf(10);
        system->planets[i].position.x = 20 * i + baseStarSize + randf(50 * i);
        system->planets[i].position.y = randf(5 * i);
        system->planets[i].position.z = 20 * i + baseStarSize + randf(50 * i);
        system->planets[i].texture = generatePlanetTexture(seed);
    }

    //Index of the planets we're putting the station next to
    uint8_t spIndex = randr(system->numPlanets);
    system->station.position.x = system->planets[spIndex].position.x + 20;
    system->station.position.y = system->planets[spIndex].position.y;
    system->station.position.z = system->planets[spIndex].position.z;
    system->station.dockingPosition.x = system->station.position.x + 5;
    system->station.dockingPosition.y = system->station.position.y;
    system->station.dockingPosition.z = system->station.position.z + 1.25f;
}