#ifndef _NPC_H
#define _NPC_H

#include "../ship.h"

typedef enum {
    STATE_IDLE,
    STATE_CIRCLE,
    STATE_ATTACK,
    STATE_FLEE,
    STATE_POLICE_CHECK,
} AiState;

typedef struct {
    Ship ship;

    AiState state;
    quat targetRot;
} Npc;

#endif