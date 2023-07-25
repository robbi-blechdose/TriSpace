#ifndef GENERATOR_DATA_H
#define GENERATOR_DATA_H

#include <stdint.h>

#include "../../engine/includes/FastNoiseLite.h"
#include "../starsystem.h"

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

typedef struct {
    Color palettes[8];
    float textureScaler;
    fnl_noise_type noiseType;
    fnl_fractal_type fractalType;
} TextureGeneratorData;

//--------- Star data --------//

extern const uint8_t starNumProbabilities[MAX_STARS];

extern const TextureGeneratorData starGeneratorData[NUM_STAR_TYPES];

//-------- Planet data -------//

typedef struct {
    TextureGeneratorData texture;
    struct {
        int8_t tree;
        int8_t rock;
        int8_t water;
    } tradeDiffs;
} PlanetGeneratorData;

//Planets are ordered by closeness to the sun
extern const PlanetGeneratorData planetGeneratorData[NUM_PLANET_TYPES];

//----- System name data -----//

extern const char* greekLetters[];
extern const char* romanNumerals[];
#define NUM_ONSETS 21
extern const char* onsets[NUM_ONSETS];
#define NUM_VOWELS_SINGLE 5
#define NUM_VOWELS 14
extern const char* vowels[NUM_VOWELS];
#define NUM_CODAS 21
extern const char* codas[NUM_CODAS];

//-- System description data -//

#define NUM_SD_PREAMBLES 3
extern const char* sdPreambles[NUM_SD_PREAMBLES];
#define NUM_SD_DESCRIPTIONS_P 3
extern const char* sdDescriptionsP[NUM_SD_DESCRIPTIONS_P];
#define NUM_SD_DESCRIPTIONS_N 2
extern const char* sdDescriptionsN[NUM_SD_DESCRIPTIONS_N];
#define NUM_SD_ADJECTIVES_P 6
extern const char* sdAdjectivesP[NUM_SD_ADJECTIVES_P];
#define NUM_SD_ADJECTIVES_N 3
extern const char* sdAdjectivesN[NUM_SD_ADJECTIVES_N];
#define NUM_SD_NOUNS 11
extern const char* sdNouns[NUM_SD_NOUNS];

#endif