#ifndef GENERATOR_DATA_H
#define GENERATOR_DATA_H

#include <stdint.h>

//-------- Planet data -------//

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

typedef struct {
    Color palettes[8];
    float textureScaler;
    struct {
        int8_t tree;
        int8_t rock;
        int8_t water;
    } tradeDiffs;
} PlanetGeneratorData;

//Palettes are ordered by closeness to the sun
#define NUM_PALETTES 8
extern const PlanetGeneratorData planetGeneratorData[NUM_PALETTES];

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