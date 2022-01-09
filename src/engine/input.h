#ifndef _INPUT_H
#define _INPUT_H

#include <SDL.h>

#define NUM_KEYS 13

typedef enum {
    //Keypad
    B_UP,
    B_DOWN,
    B_LEFT,
    B_RIGHT,
    //ABXY
    B_A,
    B_B,
    B_X,
    B_Y,
    //Shoulder buttons
    M,
    N,
    //Power, Start, Select
    Q,
    B_SELECT,
    B_START
} Key;

uint8_t handleInput();
uint8_t keyPressed(Key key);
uint8_t keyUp(Key key);

#endif