#include "input.h"

uint8_t keysLast[NUM_KEYS];
uint8_t keys[NUM_KEYS];

uint8_t handleInput()
{
    uint8_t running = 1;

    for(uint8_t i = 0; i < NUM_KEYS; i++)
    {
        keysLast[i] = keys[i];
    }

    SDL_Event event;
    while(SDL_PollEvent(&event))
    {
        if(event.type == SDL_QUIT)
        {
            running = 0;
        }
        else if(event.type != SDL_KEYDOWN && event.type != SDL_KEYUP)
        {
            continue;
        }

        uint8_t state = 0;
        if(event.type == SDL_KEYDOWN)
        {
            state = 1;
        }
        switch(event.key.keysym.sym)
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
            case SDLK_k:
            {
                keys[K] = state;
                break;
            }
            case SDLK_q:
            {
                running = 0;
                break;
            }
            case SDLK_z:
            {
                keys[Z] = state;
                break;
            }
        }
    }
    return running;
}

uint8_t keyPressed(Key key)
{
    return keys[key];
}

uint8_t keyUp(Key key)
{
    if(keysLast[key] == 1 && keys[key] == 0)
    {
        return 1;
    }
    return 0;
}