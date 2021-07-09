#include "input.h"

uint8_t keys[13];

void handleKeys(SDL_Event* event)
{
    if(event->type != SDL_KEYDOWN && event->type != SDL_KEYUP)
    {
        return;
    }

    uint8_t state = 0;
    if(event->type == SDL_KEYDOWN)
    {
        state = 1;
    }
    switch(event->key.keysym.sym)
    {
        case SDLK_u:
        {
            keys[U] = state;
            break;
        }
        case SDLK_d:
        {
            keys[D] = state;
            break;
        }
        case SDLK_l:
        {
            keys[L] = state;
            break;
        }
        case SDLK_r:
        {
            keys[R] = state;
            break;
        }
        case SDLK_a:
        {
            keys[A] = state;
            break;
        }
        case SDLK_b:
        {
            keys[B] = state;
            break;
        }
        case SDLK_x:
        {
            keys[X] = state;
            break;
        }
        case SDLK_y:
        {
            keys[Y] = state;
            break;
        }
    }
}

uint8_t keyPressed(Key key)
{
    return keys[key];
}