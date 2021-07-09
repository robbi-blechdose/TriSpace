#ifndef _INPUT_H
#define _INPUT_H

#include <SDL.h>

typedef enum {
    U,
    D,
    L,
    R,
    A,
    B,
    X,
    Y,
    M,
    N,
    Q,
    K,
    S
} Key;

void handleKeys(SDL_Event* event);
uint8_t keyPressed(Key key);

#endif