#ifndef _INPUT_H
#define _INPUT_H

#include <SDL.h>

#define NUM_KEYS 14

typedef enum {
    //Keypad
    U,
    D,
    L,
    R,
    //ABXY
    A,
    B,
    X,
    Y,
    //Shoulder buttons
    M,
    N,
    //Power, Start, Select
    Q,
    K,
    S,
    //Debug key (PC only, the FK doesn't have it)
    Z
} Key;

uint8_t handleInput();
uint8_t keyPressed(Key key);
uint8_t keyUp(Key key);

#endif