#include "generator_data.h"
#include "generator.h"

//--------- Star data --------//

const uint8_t starNumProbabilities[MAX_STARS] = {
    10,
    30,
    100
};

//-------- Planet data -------//

const PlanetGeneratorData planetGeneratorData[NUM_PALETTES] = {
    [PT_Venus] = {
        .palettes = {
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
        .textureScaler = 1.3f,
        .tradeDiffs = {.tree = -2, .rock = 2, .water = -2}
    },
    [PT_Mars] = {
        .palettes = {
            {.r = 145, .g = 69, .b = 25},
            {.r = 199, .g = 88, .b = 24},
            {.r = 227, .g = 132, .b = 36},
            {.r = 240, .g = 153, .b = 67},
            {.r = 247, .g = 163, .b = 89},
            {.r = 199, .g = 160, .b = 125},
            {.r = 199, .g = 145, .b = 97},
            {.r = 255, .g = 255, .b = 255}
        },
        .textureScaler = 1.0f,
        .tradeDiffs = {.tree = -1, .rock = 1, .water = -1}
    },
    [PT_Earth] = {
        .palettes = {
            //Ocean
            {.r = 33, .g = 13, .b = 130},
            {.r = 32, .g = 41, .b = 212},
            {.r = 122, .g = 228, .b = 240},
            {.r = 44, .g = 130, .b = 232},
            //Sand
            {.r = 250, .g = 226, .b = 92},
            //Grass
            {.r = 73, .g = 163, .b = 42},
            {.r = 105, .g = 212, .b = 68},
            {.r = 107, .g = 128, .b = 38}
        },
        .textureScaler = 0.9f,
        .tradeDiffs = {.tree = 1, .rock = -1, .water = 0}
    },
    [PT_Ocean] = {
        .palettes = {
            //Ocean
            {.r = 7, .g = 45, .b = 105},
            {.r = 29, .g = 85, .b = 173},
            {.r = 12, .g = 105, .b = 235},
            {.r = 42, .g = 76, .b = 212},
            {.r = 29, .g = 173, .b = 209},
            //Shores
            {.r = 246, .g = 255, .b = 161},
            {.r = 243, .g = 255, .b = 130},
            //Mountains
            {.r = 105, .g = 105, .b = 105}
        },
        .textureScaler = 0.5f,
        .tradeDiffs = {.tree = 0, .rock = -1, .water = 2}
    },
    [PT_Forest] = {
        .palettes = {
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
        .textureScaler = 1.0f,
        .tradeDiffs = {.tree = 2, .rock = -1, .water = 0}
    },
    [PT_Ice] = {
        .palettes = {
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
        .textureScaler = 2.0f,
        .tradeDiffs = {.tree = -2, .rock = 1, .water = 2}
    },
    [PT_Dead] = {
        .palettes = {
            {.r = 43, .g = 43, .b = 43},
            {.r = 66, .g = 66, .b = 66},
            {.r = 75, .g = 75, .b = 75},
            {.r = 90, .g = 90, .b = 90},
            {.r = 120, .g = 120, .b = 120},
            {.r = 80, .g = 80, .b = 80},
            {.r = 130, .g = 130, .b = 130},
            {.r = 150, .g = 150, .b = 150},
        },
        .textureScaler = 3.0f,
        .tradeDiffs = {.tree = -2, .rock = 2, .water = -2}
    },
    [PT_Gas] = {
        .palettes = {
            {.r = 0, .g = 41, .b = 99},
            {.r = 19, .g = 83, .b = 173},
            {.r = 12, .g = 106, .b = 237},
            {.r = 12, .g = 38, .b = 237},
            {.r = 68, .b = 29, .b = 224},
            {.r = 63, .g = 32, .b = 189},
            {.r = 90, .g = 32, .b = 189},
            {.r = 97, .g = 19, .b = 232}
        },
        .textureScaler = 0.6f,
        .tradeDiffs = {.tree = -2, .rock = -2, .water = 0}
    }
};


//----- System name data -----//

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

const char* onsets[NUM_ONSETS] = {
    "b", "c", "d", "f", "g", "h", "j", "k", "l", "m", "n", "p", "q", "r", "s", "t", "v", "w", "x", "y", "z"
};

const char* vowels[NUM_VOWELS] = {
    "a", "e", "i", "o", "u",
    "ai",
    "ea", "ee", "eu",
    "ie",
    "oa", "oi", "oo", "ou"
};

const char* codas[NUM_CODAS] = {
    "b", "c", "d", "f", "g", "h", "j", "k", "l", "m", "n", "p", "q", "r", "s", "t", "v", "w", "x", "y", "z"
};

//-- System description data -//

const char* sdPreambles[NUM_SD_PREAMBLES] = {
    "Popular with tourists, ",
    "Long thought lost, ",
    "The system "
};

const char* sdDescriptionsP[NUM_SD_DESCRIPTIONS_P] = {
    " is well known for its ",
    " is famed for its ",
    " is popular for its "
};

const char* sdDescriptionsN[NUM_SD_DESCRIPTIONS_N] = {
    " is plagued by ",
    " is infamous for its "
};

const char* sdAdjectivesP[NUM_SD_ADJECTIVES_P] = {
    "wonderful ",
    "interesting ",
    "pretty ",
    "harmless ",
    "robotic ",
    "colorful ",
};

const char* sdAdjectivesN[NUM_SD_ADJECTIVES_N] = {
    "boring ",
    "horrible ",
    "illegal "
};

const char* sdNouns[NUM_SD_NOUNS] = {
    "wildlife",
    "politicans",
    "musicians",
    "scientists",
    "insect people",
    "aliens",
    "lizards",
    "cities",
    "drugs",
    "landmarks",
    "public transport"
};