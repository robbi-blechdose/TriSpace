#include "generator.h"
#include <stdlib.h>
#include "GL/gl.h"
#define FNL_IMPL
#include "FastNoiseLite.h"

//Palettes are ordered by closeness to the sun
#define NUM_PALETTES 6
const Color palettes[NUM_PALETTES][8] = {
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
    //Ocean-type planet
    //Forest-type planet
    {
        //Ground
        {.r = 110, .g = 46, .b = 1},
        {.r = 79, .g = 110, .b = 1},
        //Trees (low)
        {.r = 97, .g = 158, .b = 17},
        {.r = 84, .g = 125, .b = 30},
        //Trees (high)
        {.r = 142, .g = 191, .b = 8},
        {.r = 185, .g = 222, .b = 82},
        //Trees (very high, snow tips)
        {.r = 175, .g = 252, .b = 73},
        {.r = 247, .g = 247, .b = 247}
    },
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
    },
    //Gas-type planet
    {
        {.r = 0, .g = 41, .b = 99},
        {.r = 19, .g = 83, .b = 173},
        {.r = 12, .g = 106, .b = 237},
        {.r = 12, .g = 38, .b = 237},
        {.r = 68, .b = 29, .b = 224},
        {.r = 63, .g = 32, .b = 189},
        {.r = 90, .g = 32, .b = 189},
        {.r = 97, .g = 19, .b = 232}
    }
};

//Diffs are: Tree, Rock, Water
const int8_t planetTradeDiffs[NUM_PALETTES][3] = {
    //Venus-type planet
    {-2, 2, -2},
    //Mars-type planet
    {-1, 1, -1},
    //Earth-type planet
    {1, -1, 0},
    //Ocean-type planet
    //Forest-type planet
    {2, -1, 0},
    //Ice-type planet
    {-2, 2, 2},
    //Gas-type planet
    {-2, -2, 0}
};

Color getColorForValue(uint8_t paletteIndex, float value)
{
    //Get an index between 0 and 8
    uint8_t index = value / 32;
    Color a = palettes[paletteIndex][index];
    Color b = palettes[paletteIndex][index + 1];
    Color ret;
    ret.r = a.r + (b.r - a.r) * ((value / 32) - index);
    ret.g = a.g + (b.g - a.g) * ((value / 32) - index);
    ret.b = a.b + (b.b - a.b) * ((value / 32) - index);
    return ret;
}

GLuint generatePlanetTexture(uint32_t seed, uint8_t paletteIndex)
{
    uint8_t data[256 * 256 * 3];
    float size = 2 + randf(6);

    //Generate texture
    fnl_state noise = fnlCreateState();
    noise.noise_type = FNL_NOISE_VALUE;
    noise.fractal_type = FNL_FRACTAL_FBM;
    noise.seed = seed + rand();
    for(uint16_t i = 0; i < 256; i++)
    {
        for(uint16_t j = 0; j < 256; j++)
        {
            //Noise value (-1 to 1) is scaled to be within 0 to 256
            float temp = (fnlGetNoise2D(&noise, i * size, j * size) + 1.0f) * 128;
            Color c = getColorForValue(paletteIndex, temp);
            uint32_t index = i * 256 * 3 + j * 3;
            data[index] = c.r;
            data[index + 1] = c.g;
            data[index + 2] = c.b;
        }
    }

    //Texture seam fix
    for(uint16_t i = 0; i < 256; i++)
    {
        for(uint16_t j = 0; j < 16; j++)
        {
            //Noise value (-1 to 1) is scaled to be within 0 to 256
            float temp = (fnlGetNoise2D(&noise, i * size, 255 * size + j * size) + 1.0f) * 128;
            Color c = getColorForValue(paletteIndex, temp);
            uint32_t index = i * 256 * 3 + j * 3;
            float multA = j / 16.0f;
            float multB = (16 - j) / 16.0f;
            data[index] = (c.r * multB) + (data[index] * multA);
            data[index + 1] = (c.g * multB) + (data[index + 1] * multA);
            data[index + 2] = (c.b * multB) + (data[index + 2] * multA);
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
    info->techLevel = 1 + randr(MAX_TECH_LEVEL - 1);
    info->government = ((float) info->techLevel / MAX_TECH_LEVEL) * 2 + randr(MAX_GOVERNMENT - 2);
    info->treeDiff = planetTradeDiffs[paletteIndex][0];
    info->rockDiff = planetTradeDiffs[paletteIndex][1];
    info->waterDiff = planetTradeDiffs[paletteIndex][2];
    generateSystemName((char*) &info->name);
}

uint8_t getNumStarsForSystem(uint32_t seed)
{
    srand(seed);
    uint32_t numStarsTemp = randr(100);
    if(numStarsTemp < 15)
    {
        return 3;
    }
    else if(numStarsTemp < 40)
    {
        return 2;
    }
    else
    {
        return 1;
    }
}

void generateSystemBaseData(SystemBaseData* sbd, uint32_t seed)
{
    //This takes care of the srand() call for us
    sbd->numStars = getNumStarsForSystem(seed);
    sbd->numPlanets = 1 + randr(MAX_PLANETS - 1);
    sbd->spIndex = randr(sbd->numPlanets - 1);
    for(uint8_t i = 0; i < sbd->numPlanets; i++)
    {
        //TODO: Make this scale (e.g. a 1-planet system should also be able to have gas planets)
        sbd->paletteIndices[i] = (i + randr(2)) % NUM_PALETTES;
    }
    generateSystemInfo(&sbd->info, sbd->paletteIndices[sbd->spIndex]);
}

void generateStarSystem(StarSystem* system, uint32_t seed)
{
    //This takes care of the srand() call for us
    SystemBaseData sbd;
    generateSystemBaseData(&sbd, seed);
    system->info.techLevel = sbd.info.techLevel;
    system->info.government = sbd.info.government;
    system->info.treeDiff = sbd.info.treeDiff;
    system->info.rockDiff = sbd.info.rockDiff;
    system->info.waterDiff = sbd.info.waterDiff;

    system->numStars = sbd.numStars;
    system->numPlanets = sbd.numPlanets;

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
            firstOrbit = baseStarSize * 6;
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

    for(uint8_t i = 0; i < system->numPlanets; i++)
    {
        system->planets[i].size = 10.0f + randf(10);

        int8_t positive = randr(2) * 2 - 1;
        uint8_t useX = randr(10) < 5;

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
        system->planets[i].position.y = randf(20 * i) - 10 * i;
        system->planets[i].texture = generatePlanetTexture(seed, sbd.paletteIndices[i]);
        system->planets[i].hasRing = randr(100) < 30;
    }

    system->station.position.x = system->planets[sbd.spIndex].position.x + system->planets[sbd.spIndex].size * 2;
    system->station.position.y = system->planets[sbd.spIndex].position.y;
    system->station.position.z = system->planets[sbd.spIndex].position.z;
    system->station.dockingPosition.x = system->station.position.x + 5;
    system->station.dockingPosition.y = system->station.position.y;
    system->station.dockingPosition.z = system->station.position.z + 1.25f;
    system->station.exitPosition.x = system->station.position.x + 7;
    system->station.exitPosition.y = system->station.position.y;
    system->station.exitPosition.z = system->station.position.z + 3;
}

void generateSystemSeeds(uint32_t* systemSeeds, uint32_t baseSeed)
{
    srand(baseSeed);
    for(uint16_t i = 0; i < 256; i++)
    {
        systemSeeds[i] = rand();
    }
}

void generateSystemPos(vec2* systemPos, uint32_t seed, uint8_t i, uint8_t j)
{
    srand(seed);
    systemPos->x = (float) i * 64 + (randf(48) - 24);
    systemPos->y = (float) j * 64 + (randf(48) - 24);
}

const char* greekLetters[] = {
    "Alpha ",
    "Beta ",
    "Gamma ",
    "Delta ",
    "Epsilon ",
    "Zeta ",
    "Eta ",
    "Theta ",
    "Iota ",
    "Kappa ",
    "Lambda ",
    "Mu ",
    "Nu ",
    "Xi ",
    "Omicron "
    "Pi ",
    "Rho ",
    "Sigma ",
    "Tau ",
    "Upsilon ",
    "Phi ",
    "Chi ",
    "Psi ",
    "Omega "
};

const char* romanNumerals[] = {
    " I",
    " II",
    " III",
    " IV",
    " V",
    " VI",
    " VII",
    " VIII",
    " IX"
};

#define NUM_ONSETS 21
const char* onsets[NUM_ONSETS] = {
    "b", "c", "d", "f", "g", "h", "j", "k", "l", "m", "n", "p", "q", "r", "s", "t", "v", "w", "x", "y", "z"
};

#define NUM_VOWELS_SINGLE 5
#define NUM_VOWELS 14
const char* vowels[NUM_VOWELS] = {
    "a", "e", "i", "o", "u",
    "ai",
    "ea", "ee", "eu",
    "ie",
    "oa", "oi", "oo", "ou"
};

#define NUM_CODAS 21
const char* codas[NUM_CODAS] = {
    "b", "c", "d", "f", "g", "h", "j", "k", "l", "m", "n", "p", "q", "r", "s", "t", "v", "w", "x", "y", "z"
};

void generateSystemName(char* buffer)
{
    //Generate prefix (optional)
    uint8_t prefix = randr(50);
    if(prefix < 23)
    {
        strcpy(buffer, greekLetters[prefix]);
    }

    char name[20];
    uint8_t nameIndex = 0;
    //Generate name
    uint8_t syllables = 1 + randr(3);
    uint8_t doubleVowelOccurred = 0;
    //Syllable (x1 - x4)
    for(uint8_t i = 0; i < syllables; i++)
    {
        //Onset (optional)
        uint8_t oIndex = randr(NUM_ONSETS - 1);
        strcpy(&name[nameIndex], onsets[oIndex]);
        nameIndex += strlen(onsets[oIndex]);
        //Nucleus
        uint8_t vIndex = randr(NUM_VOWELS - 1);
        if(doubleVowelOccurred)
        {
            vIndex = randr(NUM_VOWELS_SINGLE - 1);
        }
        strcpy(&name[nameIndex], vowels[vIndex]);
        nameIndex += strlen(vowels[vIndex]);
        if(vIndex > 4)
        {
            doubleVowelOccurred = 1;
        }
        //Coda (optional)
        uint8_t cIndex = randr(NUM_CODAS * 4);
        if(cIndex < NUM_CODAS)
        {
            strcpy(&name[nameIndex], codas[cIndex]);
            nameIndex += strlen(codas[cIndex]);
        }

        //printf("Onset: %d Nucleus: %d DVO: %d\n", oIndex, vIndex, doubleVowelOccurred);
    }
    //Make the first letter uppercase
    name[0] &= ~0x20;
    //Append if we have a prefix, otherwise overwrite the buffer
    if(prefix < 23)
    {
        strcat(buffer, name);
    }
    else
    {
        strcpy(buffer, name);
    }

    //Generate number (optional)
    uint8_t num = randr(9 * 3);
    if(num < 9)
    {
        strcat(buffer, romanNumerals[num]);
    }
    //printf("Prefix: %d Syllables: %d Number: %d\n", prefix, syllables, num);
}