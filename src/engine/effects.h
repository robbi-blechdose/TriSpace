#ifndef _EFFECTS_H
#define _EFFECTS_H

#include <SDL.h>
#include "GL/gl.h"
#include "util.h"

#define MAX_EFFECTS 64

typedef struct {
    vec3 position;
    uint8_t frames;
    uint8_t startFrame;
    uint8_t frameIndex;
    uint8_t frameTime;
    int32_t life;
    float size;
} Effect;

typedef enum {
    EXPLOSION,
    SPARKS
} EffectType;

void initEffects();
uint8_t createEffect(vec3 position, EffectType type);
void calcEffects(uint32_t ticks);
void drawEffects();

#endif