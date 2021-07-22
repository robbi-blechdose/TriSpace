#include <SDL.h>
#define CHAD_API_IMPL
#include "zbuffer.h"
#include "engine/includes/3dMath.h"
#include "engine/input.h"
#include "engine/camera.h"
#include "engine/view.h"

#include "ui.h"
#include "ship.h"
#include "universe/universe.h"

#define WINX 240
#define WINY 240
#define WINY_3D (WINY - 70)
#define MAX_FPS 50
//#define LIMIT_FPS

SDL_Surface* screen;
ZBuffer* frameBuffer = NULL;
SDL_Event event;
uint16_t fps;

uint8_t counterEnabled = 0;
uint32_t counterFrames = 0;
uint32_t counterTime = 0;

uint8_t running = 1;

//---------- Main game stuff ----------//
State state;
State targetState;

Ship npcShips[MAX_NPC_SHIPS];

//Temporary (TODO: REMOVE)
ShipType test = {.maxSpeed = 10, .maxTurnSpeed = 5, .maxShields = 10, .maxEnergy = 10, .shieldRegen = 1, .energyRegen = 1};

Ship playerShip;
//-------------------------------------//

void drawFPS(uint16_t fps)
{
    char buffer[12];
	sprintf(buffer, "FPS: %i", fps);
	glDrawText(buffer, 3, 3, 0xFFFFFF);
}

void handleInput(uint16_t ticks)
{
    if(event.type == SDL_KEYUP)
    {
        switch(event.key.keysym.sym)
        {
            case SDLK_q:
            {
                running = 0;
                break;
            }
            case SDLK_c:
            {
                counterEnabled = !counterEnabled;
                if(!counterEnabled)
                {
                    printf("%d frames in %d ms. %f fps. %f ms/frame.\n", counterFrames, counterTime,
                                                                ((float) counterFrames / counterTime) * 1000.0f,
                                                                (float) counterTime / counterFrames);
                    counterTime = 0;
                    counterFrames = 0;
                }
                break;
            }
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
    if(counterEnabled)
    {
        counterTime += ticks;
        counterFrames++;
    }

    if(keyPressed(X))
    {
        accelerateShip(&playerShip, 1, ticks);
    }
    else if(keyPressed(Y))
    {
        accelerateShip(&playerShip, -1, ticks);
    }
    int8_t dirX = 0;
    int8_t dirY = 0;
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

    calcUniverse(&state, &targetState, &playerShip, npcShips);
}

void drawFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);

    drawCamera();

    drawUniverse(&state, npcShips);

    drawFPS(fps);

    setOrtho();
    drawUI(state, &playerShip, npcShips);
    //drawTradingUI(&playerShip, 0);
    setPerspective();

    if(SDL_MUSTLOCK(screen))
    {
        SDL_LockSurface(screen);
    }
    ZB_copyFrameBuffer(frameBuffer, screen->pixels, screen->pitch);
	if(SDL_MUSTLOCK(screen))
    {
		SDL_UnlockSurface(screen);
    }
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
    glViewport(0, 0, WINX, WINY_3D);
	glTextSize(GL_TEXT_SIZE8x8);
	glEnable(GL_TEXTURE_2D);

    float winPersp[] = {WINX, WINY_3D};
    float winOrtho[] = {WINX, WINY};
    float clipPersp[] = {1, 512};
    float clipOrtho[] = {-15, 0};
    initView(70, winPersp, winOrtho, clipPersp, clipOrtho);
    setPerspective();

    //Initialize game
    state = SPACE;
    targetState = NONE;
    initUI();
    initUniverse();
    initShip();

    //Temporary (TODO: REMOVE)
    playerShip.type = &test;
    playerShip.position.x = 150;
    playerShip.position.z = 100;
    npcShips[0].type = &test;
    npcShips[0].position.x = 140;
    npcShips[0].position.z = 100;

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

        #ifdef LIMIT_FPS
		if((1000 / MAX_FPS) > (SDL_GetTicks() - tNow))
        {
			SDL_Delay((1000 / MAX_FPS) - (SDL_GetTicks() - tNow)); // Yay stable framerate!
		}
        #endif
		fps = 1000.0f / (float)(tNow - tLastFrame);
		tLastFrame = tNow;
    }
    
    //TinyGL cleanup
	ZB_close(frameBuffer);
	glClose();

    SDL_Quit();

	return 0;
}