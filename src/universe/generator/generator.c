#include "generator.h"
#include <stdlib.h>
#include "GL/gl.h"
#define FNL_IMPL
#include "../../engine/includes/FastNoiseLite.h"
#include "../asteroids.h"
#include "generator_data.h"

Color getColorForValue(uint8_t paletteIndex, float value)
{
    //Get an index between 0 and 8
    uint8_t index = value / 32;
    Color a = planetGeneratorData[paletteIndex].palettes[index];
    Color b = planetGeneratorData[paletteIndex].palettes[index + 1];
    Color ret;
    ret.r = a.r + (b.r - a.r) * ((value / 32) - index);
    ret.g = a.g + (b.g - a.g) * ((value / 32) - index);
    ret.b = a.b + (b.b - a.b) * ((value / 32) - index);
    return ret;
}

GLuint generatePlanetTexture(uint32_t seed, uint8_t paletteIndex)
{
    uint8_t data[256 * 256 * 3];
    float size = (2 + randf(5)) * planetGeneratorData[paletteIndex].textureScaler;

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
    info->government = ((float) info->techLevel / MAX_TECH_LEVEL) * 2 + randr(MAX_GOVERNMENT - 3);
    info->treeDiff = planetGeneratorData[paletteIndex].tradeDiffs.tree;
    info->rockDiff = planetGeneratorData[paletteIndex].tradeDiffs.rock;
    info->waterDiff = planetGeneratorData[paletteIndex].tradeDiffs.water;
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
        sbd->paletteIndices[i] = (i + randr(NUM_PALETTES - sbd->numPlanets)) % NUM_PALETTES;
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
    float firstOrbit = baseStarSize * 3;

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
        system->planets[i].texture = generatePlanetTexture(seed, sbd.paletteIndices[i]);
        system->planets[i].hasRing = randr(100) < 30;

        float orbitRadius = firstOrbit + (i * 35.0f);
        float angle = randf(M_PI * 2);
        system->planets[i].position.x = orbitRadius * cos(angle);
        system->planets[i].position.z = orbitRadius * sin(angle);
        system->planets[i].position.y = randf(40 * i) - 20 * i;
    }

    //Space station generation
    system->station.position.x = system->planets[sbd.spIndex].position.x + system->planets[sbd.spIndex].size * 2;
    system->station.position.y = system->planets[sbd.spIndex].position.y;
    system->station.position.z = system->planets[sbd.spIndex].position.z;
    system->station.dockingPosition.x = system->station.position.x + 5;
    system->station.dockingPosition.y = system->station.position.y;
    system->station.dockingPosition.z = system->station.position.z + 1.25f;
    system->station.exitPosition.x = system->station.position.x + 7;
    system->station.exitPosition.y = system->station.position.y;
    system->station.exitPosition.z = system->station.position.z + 3;

    //Asteroid field generation
    if(randr(100) < 40)
    {
        system->hasAsteroidField = 1;
        system->asteroidFieldPos = getRandomFreePos(system, ASTEROID_FIELD_SIZE);
    }
    else
    {
        system->hasAsteroidField = 0;
    }
}

vec2 generateSystemPos(uint32_t seed, uint8_t i, uint8_t j)
{
    srand(seed);
    //TODO: improve?
    return (vec2) {i * 10 + (randf(4) - 2), j * 10 + (randf(4) - 2)};
}

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
    uint8_t syllables = 1 + randr(2);
    uint8_t doubleVowelOccurred = 0;
    //Syllable (x1 - x3)
    for(uint8_t i = 0; i < syllables; i++)
    {
        //Onset
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

void generateSystemDescription(char* buffer, SystemBaseData* sbd)
{
    //Preamble (optional) and name
    if(randr(100) < 40)
    {
        strcpy(buffer, sdPreambles[randr(NUM_SD_PREAMBLES - 1)]);
        strcat(buffer, sbd->info.name);
    }
    else
    {
        strcpy(buffer, sbd->info.name);
    }

    //Description beginning, adjective
    bool positive = randr(100) < 50;
    if(positive)
    {
        strcat(buffer, sdDescriptionsP[randr(NUM_SD_DESCRIPTIONS_P - 1)]);
        strcat(buffer, sdAdjectivesP[randr(NUM_SD_ADJECTIVES_P - 1)]);
    }
    else
    {
        strcat(buffer, sdDescriptionsN[randr(NUM_SD_DESCRIPTIONS_N - 1)]);
        strcat(buffer, sdAdjectivesN[randr(NUM_SD_ADJECTIVES_N - 1)]);
    }

    //Add noun
    strcat(buffer, sdNouns[randr(NUM_SD_NOUNS - 1)]);

    strcat(buffer, ". ");

    //Add linebreaks where necessary
    uint8_t index = 0;
    for(int8_t i = 0; i < 3; i++)
    {
        for(uint8_t j = index + 29 - 1; j > index; j--)
        {
            if(buffer[j] == ' ')
            {
                buffer[j] = '\n';
                index = j + 1;
                break;
            }
        }
    }
}