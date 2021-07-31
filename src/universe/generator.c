#include "generator.h"
#include <stdlib.h>
#include "GL/gl.h"
#define FNL_IMPL
#include "FastNoiseLite.h"

#define NUM_PALETTES 4
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
    },
    //Venus-type planet
    {
        //Base colors
        {.r = 41, .g = 6, .b = 4},
        {.r = 99, .g = 14, .b = 10},
        {.r = 133, .g = 72, .b = 3},
        //Lava
        {.r = 255, .g = 62, .b = 23},
        {.r = 255, .g = 147, .b = 23},
        {.r = 255, .g = 198, .b = 10},
        //Mountains
        {.r = 209, .g = 168, .b = 121},
        {.r = 69, .g = 62, .b = 45},
    },
    //Ocean-type planet
    //Forest-type planet
    //Ice-type planet
    {
        //Frozen oceans
        {.r = 51, .g = 181, .b = 181},
        {.r = 54, .g = 129, .b = 191},
        {.r = 107, .g = 182, .b = 214},
        {.r = 145, .g = 209, .b = 237},
        //Mountains
        {.r = 85, .g = 86, .b = 87},
        {.r = 61, .g = 61, .b = 61},
        //Snow caps
        {.r = 224, .g = 224, .b = 224},
        {.r = 230, .g = 240, .b = 245}
    }
};

//Diffs are: Tree, Rock, Water
int8_t planetTradeDiffs[NUM_PALETTES][3] = {
    //Earth-type planet
    {1, -1, 0},
    //Mars-type planet
    {-1, 1, -1},
    //Venus-type planet
    {-2, 2, -2},
    //Ocean-type planet
    //Forest-type planet
    //Ice-type planet
    {-2, 2, 2}
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

//TODO: Fix texture seam in x direction (y technically has a seam but that's unimportant)
GLuint generatePlanetTexture(uint32_t seed, uint8_t paletteIndex)
{
    uint8_t data[256 * 256 * 3];
    float size = 2 + randf(4);

    //Generate texture
    fnl_state noise = fnlCreateState();
    noise.noise_type = FNL_NOISE_VALUE;
    noise.fractal_type = FNL_FRACTAL_FBM;
    noise.seed = seed + rand();
    for(uint16_t i = 0; i < 256; i++)
    {
        for(uint16_t j = 0; j < 256; j++)
        {
            float temp = (fnlGetNoise2D(&noise, i * size, j * size) + 1.0f) * 128;
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

void generateSystemInfo(SystemInfo* info, uint8_t paletteIndex)
{
    info->techLevel = 1 + randr(MAX_TECH_LEVEL);
    info->treeDiff = planetTradeDiffs[paletteIndex][0];
    info->rockDiff = planetTradeDiffs[paletteIndex][1];
    info->waterDiff = planetTradeDiffs[paletteIndex][2];
}

uint8_t getNumStarsForSystem(uint32_t seed)
{
    srand(seed);
    return 1 + randr(3);
}

void generateStarSystem(StarSystem* system, uint32_t seed)
{
    srand(seed);

    system->numStars = 1 + randr(3);
    system->numPlanets = 1 + randr(9);

    float baseStarSize = 40 + randf(30);
    float firstOrbit = baseStarSize * 2;

    uint8_t xUsed = 0;
    for(uint8_t i = 0; i < system->numStars; i++)
    {
        system->stars[i].size = baseStarSize + randf(5);
        system->stars[i].position.x = 0;
        system->stars[i].position.y = 0;
        system->stars[i].position.z = 0;
        //Create binary or trinary star systems
        if(i > 0)
        {
            firstOrbit = baseStarSize * 5;
            if(!xUsed)
            {
                system->stars[i].position.x = baseStarSize * 2;
                xUsed = 1;
            }
            else
            {
                system->stars[i].position.z = baseStarSize * 2;
            }
        }
    }

    //Index of the planet we're putting the station next to
    uint8_t spIndex = randr(system->numPlanets);
    uint8_t spPaletteIndex;

    for(uint8_t i = 0; i < system->numPlanets; i++)
    {
        system->planets[i].size = 10.0f + randf(10);

        int8_t positive = randr(2) * 2 - 1;
        uint8_t useX = randr(2);

        if(useX)
        {
            system->planets[i].position.x = (firstOrbit + (30 * i) + randf(50 * i)) * positive;
            system->planets[i].position.z = randf(50) - 25;
        }
        else
        {
            system->planets[i].position.x = randf(50) - 25;
            system->planets[i].position.z = (firstOrbit + (30 * i) + randf(50 * i)) * positive;
        }
        system->planets[i].position.y = randf(5 * i);
        uint8_t paletteIndex = randr(NUM_PALETTES);
        system->planets[i].texture = generatePlanetTexture(seed, paletteIndex);
        //Save palette index
        if(i == spIndex)
        {
            spPaletteIndex = paletteIndex;
        }

        system->planets[i].hasRing = randr(100) < 30;
    }

    system->station.position.x = system->planets[spIndex].position.x + system->planets[spIndex].size * 2;
    system->station.position.y = system->planets[spIndex].position.y;
    system->station.position.z = system->planets[spIndex].position.z;
    system->station.dockingPosition.x = system->station.position.x + 5;
    system->station.dockingPosition.y = system->station.position.y;
    system->station.dockingPosition.z = system->station.position.z + 1.25f;
    system->station.exitPosition.x = system->station.position.x + 7;
    system->station.exitPosition.y = system->station.position.y;
    system->station.exitPosition.z = system->station.position.z + 3;

    generateSystemInfo(&system->info, spPaletteIndex);
}

void generateSystemSeeds(uint32_t* systemSeeds, uint32_t baseSeed)
{
    srand(baseSeed);
    for(uint16_t i = 0; i < 256; i++)
    {
        systemSeeds[i] = rand();
    }
}