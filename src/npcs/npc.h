#ifndef NPC_H
#define NPC_H

#include "../ship.h"

typedef enum {
    STATE_IDLE,
    STATE_CIRCLE,
    STATE_ATTACK,
    STATE_FLEE
} AiState;

typedef struct {
    Ship ship;

    AiState state;
    vec3 waypoint;
} Npc;

#endif