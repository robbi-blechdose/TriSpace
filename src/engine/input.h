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

uint8_t handleInput();
uint8_t keyPressed(Key key);
uint8_t keyUp(Key key);

#endif