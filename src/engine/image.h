#ifndef _IMAGE_H
#define _IMAGE_H

#include "GL/gl.h"
#include <SDL.h>

void initPNG();
SDL_Surface* loadPNG(const char* path);
GLuint loadRGBTexture(unsigned char* path);
void deleteRGBTexture(GLuint texture);

void blitSurface(SDL_Surface* target, SDL_Surface* source, uint8_t x, uint8_t y);

#endif