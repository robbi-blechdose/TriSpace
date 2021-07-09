#include <SDL.h>
#define CHAD_API_IMPL
#include "zbuffer.h"
#include "engine/includes/3dMath.h"
#include "engine/input.h"
#include "engine/camera.h"

#include "starsystem.h"
#include "ui.h"
#include "ship.h"

#define WINX 240
#define WINY 240
#define MAX_FPS 50

SDL_Surface* screen;
ZBuffer* frameBuffer = NULL;
SDL_Event event;
uint16_t fps;

uint8_t running = 1;

//---------- Main game stuff ----------//

//Temporary (TODO: REMOVE)
ShipType test = {.maxSpeed = 5};

Ship playerShip;
//-------------------------------------//

void drawFPS(uint16_t fps)
{
    char buffer[12];
	sprintf(buffer, "FPS: %i", fps);
	glDrawText(buffer, 1, 1, 0xFFFFFF);
}

void handleInput(uint16_t ticks)
{
    if(event.type == SDL_KEYUP)
    {
        switch(event.key.keysym.sym)
        {
            default:
            {
                break;
            }
        }
    }
    else if(event.type == SDL_QUIT)
    {
        running = 0;
    }
}

void calcFrame(uint32_t ticks)
{
    if(keyPressed(X))
    {
        accelerateShip(&playerShip, 1, ticks);
    }
    else if(keyPressed(Y))
    {
        accelerateShip(&playerShip, -1, ticks);
    }
    int8_t dirX, dirY;
    if(keyPressed(U))
    {
        dirX = 1;
    }
    else if(keyPressed(D))
    {
        dirX = -1;
    }
    if(keyPressed(L))
    {
        dirY = -1;
    }
    else if(keyPressed(R))
    {
        dirY = 1;
    }
    steerShip(&playerShip, dirX, dirY, ticks);

    calcShip(&playerShip, ticks);
    setCameraPos(playerShip.position);
    setCameraRot(playerShip.rotation);
}

void drawFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);

    drawCamera();

    drawStarSystem();

    drawFPS(fps);

    if(SDL_MUSTLOCK(screen))
    {
        SDL_LockSurface(screen);
    }
    ZB_copyFrameBuffer(frameBuffer, screen->pixels, screen->pitch);
	if(SDL_MUSTLOCK(screen))
    {
		SDL_UnlockSurface(screen);
    }
    drawUI(screen, &playerShip);
	SDL_Flip(screen);
}

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    screen = SDL_SetVideoMode(WINX, WINY, 16, SDL_SWSURFACE);
	SDL_ShowCursor(SDL_DISABLE);

    //Initialize TinyGL
	frameBuffer = ZB_open(WINX, WINY, ZB_MODE_5R6G5B, 0);
	glInit(frameBuffer);
	glShadeModel(GL_FLAT);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glClearColor(0, 0, 0, 0);
	glClearDepth(1.0f);
    glViewport(0, 0, WINX, WINY);
	glTextSize(GL_TEXT_SIZE8x8);
	glEnable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	mat4 matrix = perspective(70, (float)WINX / (float)WINY, 1, 512);
	glLoadMatrixf(matrix.d);
	glMatrixMode(GL_MODELVIEW);

    //Initialize game
    initUI();
    initStarSystem();
    loadStarSystem();

    //Temporary (TODO: REMOVE)
    playerShip.type = &test;

    //Run main loop
	uint32_t tNow = SDL_GetTicks();
	uint32_t tLastFrame = tNow;
    uint16_t ticks = 0;
    while(running)
    {
		tNow = SDL_GetTicks();
        ticks = tNow - tLastFrame;
        
        while(SDL_PollEvent(&event))
        {
            handleKeys(&event);
            handleInput(ticks);
        }

        calcFrame(ticks);
        drawFrame();

        /**
		if((1000 / MAX_FPS) > (SDL_GetTicks() - tNow))
        {
			SDL_Delay((1000 / MAX_FPS) - (SDL_GetTicks() - tNow)); // Yay stable framerate!
		}**/
		fps = 1000.0f / (float)(tNow - tLastFrame);
		tLastFrame = tNow;
    }
    
    //TinyGL cleanup
	ZB_close(frameBuffer);
	glClose();

    SDL_Quit();

	return 0;
}