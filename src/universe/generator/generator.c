#include "generator.h"
#include <stdlib.h>
#include "GL/gl.h"
#define FNL_IMPL
#include "../../engine/includes/FastNoiseLite.h"
#include "../asteroids.h"
#include "generator_data.h"

void generateSystemCharacteristics(SystemCharacteristics* chars, uint8_t stationPlanetIndex)
{
    chars->techLevel = 1 + randr(MAX_TECH_LEVEL - 1);
    chars->government = ((float) chars->techLevel / MAX_TECH_LEVEL) * 2 + randr(MAX_GOVERNMENT - 3);
    chars->treeDiff = planetGeneratorData[stationPlanetIndex].tradeDiffs.tree;
    chars->rockDiff = planetGeneratorData[stationPlanetIndex].tradeDiffs.rock;
    chars->waterDiff = planetGeneratorData[stationPlanetIndex].tradeDiffs.water;
}

uint8_t getNumStars()
{
    uint32_t randTemp = randr(100);
    uint8_t count = MAX_STARS;
    for(uint8_t i = 0; i < MAX_STARS; i++)
    {
        if(randTemp < starNumProbabilities[i])
        {
            return count;
        }
        count--;
    }
    return 1;
}

void generateStarSystemInfo(StarSystemInfo* info, uint32_t seed)
{
    srand(seed);

    //Name
    generateSystemName((char*) &info->name);

    //Stars
    info->numStars = getNumStars();
    for(uint8_t i = 0; i < info->numStars; i++)
    {
        uint8_t value = randr(100);
        if(value < 15)
        {
            info->starTypes[i] = ST_RED;
        }
        else if(value < 30)
        {
            info->starTypes[i] = ST_BLUE;
        }
        else
        {
            info->starTypes[i] = ST_NORMAL;
        }
    }

    //Planets
    info->numPlanets = 1 + randr(MAX_PLANETS - 1);
    for(uint8_t i = 0; i < info->numPlanets; i++)
    {
        info->planetTypes[i] = (i + randr(NUM_PLANET_TYPES - info->numPlanets)) % NUM_PLANET_TYPES;
    }

    //Space station
    info->stationPlanetIndex = randr(info->numPlanets - 1);

    //Asteroid field
    info->hasAsteroidField = randr(100) < 40;

    //System info
    generateSystemCharacteristics(&info->characteristics, info->planetTypes[info->stationPlanetIndex]);
}

Color getColorForValue(TextureGeneratorData tgd, float value)
{
    //Get an index between 0 and 8
    uint8_t index = value / 32;
    Color a = tgd.palettes[index];
    Color b = tgd.palettes[index + 1];
    Color ret;
    ret.r = a.r + (b.r - a.r) * ((value / 32) - index);
    ret.g = a.g + (b.g - a.g) * ((value / 32) - index);
    ret.b = a.b + (b.b - a.b) * ((value / 32) - index);
    return ret;
}

#define TEXTURE_SIZE 256
GLuint generateSphereTexture(uint32_t seed, TextureGeneratorData tgd)
{
    uint8_t data[TEXTURE_SIZE * TEXTURE_SIZE * 3];
    float size = (2 + randf(5)) * tgd.textureScaler;

    //Generate texture
    fnl_state noise = fnlCreateState();
    noise.noise_type = tgd.noiseType;
    noise.fractal_type = tgd.fractalType;
    noise.seed = seed + rand();
    for(uint16_t i = 0; i < TEXTURE_SIZE; i++)
    {
        for(uint16_t j = 0; j < TEXTURE_SIZE; j++)
        {
            //Noise value (-1 to 1) is scaled to be within 0 to 256
            float temp = (fnlGetNoise2D(&noise, i * size, j * size) + 1.0f) * 128;
            Color c = getColorForValue(tgd, temp);
            uint32_t index = i * TEXTURE_SIZE * 3 + j * 3;
            data[index] = c.r;
            data[index + 1] = c.g;
            data[index + 2] = c.b;
        }
    }

    //Texture seam fix
    for(uint16_t i = 0; i < TEXTURE_SIZE; i++)
    {
        for(uint16_t j = 0; j < 16; j++)
        {
            //Noise value (-1 to 1) is scaled to be within 0 to 256
            float temp = (fnlGetNoise2D(&noise, i * size, 255 * size + j * size) + 1.0f) * 128;
            Color c = getColorForValue(tgd, temp);
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
	glTexImage2D(GL_TEXTURE_2D, 0, 3, TEXTURE_SIZE, TEXTURE_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	return t;
}

GLuint generateStarTexture(StarType type)
{
    #define STAR_TEXTURE_SEED 1
    return generateSphereTexture(STAR_TEXTURE_SEED, starGeneratorData[type]);
}

void generateStarSystem(StarSystem* system, uint32_t seed)
{
    //The srand() call happens in here
    generateStarSystemInfo(&system->info, seed);

    //Star generation
    float baseStarSize = 40 + randf(30);
    float firstOrbit = baseStarSize * 3;
    uint8_t xUsed = 0;

    for(uint8_t i = 0; i < system->info.numStars; i++)
    {
        system->stars[i].size = baseStarSize + randf(5);
        system->stars[i].position = (vec3) {0, 0, 0};
        //TODO: replace with proper "orient on a circle" code (see the code in starmap.c)
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

    //Planet generation
    for(uint8_t i = 0; i < system->info.numPlanets; i++)
    {
        system->planets[i].size = 10.0f + randf(10);
        system->planets[i].texture = generateSphereTexture(seed, planetGeneratorData[system->info.planetTypes[i]].texture);
        system->planets[i].hasRing = randr(100) < 30;

        float orbitRadius = firstOrbit + (i * 35.0f);
        float angle = randf(M_PI * 2);
        system->planets[i].position.x = orbitRadius * cos(angle);
        system->planets[i].position.z = orbitRadius * sin(angle);
        system->planets[i].position.y = randf(40 * i) - 20 * i;
    }

    //Space station generation
    vec3 stationPlanetPos = system->planets[system->info.stationPlanetIndex].position;
    system->station.position = addv3(stationPlanetPos, (vec3) {system->planets[system->info.stationPlanetIndex].size * 2, 0, 0});
    system->station.dockingPosition = addv3(system->station.position, (vec3) {5, 0, 1.25f});
    system->station.exitPosition = addv3(system->station.position, (vec3) {7, 0, 3});

    //Asteroid field generation
    if(system->info.hasAsteroidField)
    {
        system->asteroidFieldPos = getRandomFreePos(system, ASTEROID_FIELD_SIZE);
    }
}

vec2 generateSystemPos(uint32_t seed, uint8_t i, uint8_t j)
{
    srand(seed);
    return (vec2) {i * 10 + (randf(3) - 1.5f), j * 10 + (randf(3) - 1.5f)};
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

void generateSystemDescription(char* buffer, StarSystemInfo* info)
{
    //Preamble (optional) and name
    if(randr(100) < 40)
    {
        strcpy(buffer, sdPreambles[randr(NUM_SD_PREAMBLES - 1)]);
        strcat(buffer, info->name);
    }
    else
    {
        strcpy(buffer, info->name);
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