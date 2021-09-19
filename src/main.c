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
#include "universe/starsystem.h"

//Compile with debug functionality
#define DEBUG

#define WINX 240
#define WINY 240
#define WINY_3D (WINY - 70)
#define MAX_FPS 50
//#define LIMIT_FPS

SDL_Surface* screen;
ZBuffer* frameBuffer = NULL;
SDL_Event event;
uint16_t fps;

#ifdef DEBUG
uint8_t counterEnabled = 0;
uint32_t counterFrames = 0;
uint32_t counterTime = 0;
#endif

uint8_t running = 1;

//---------- Main game stuff ----------//
State state;
State targetState;

StarSystem starSystem;
uint8_t targetSystem = 0;

CargoHold stationHold;

Ship playerShip;
Ship npcShips[MAX_NPC_SHIPS];

//Temporary (TODO: REMOVE)
ShipType test = {.maxSpeed = 10, .maxTurnSpeed = 5, .maxShields = 5, .maxEnergy = 5, .shieldRegen = 1, .energyRegen = 1};
WeaponType testWeapon = {.cooldown = 400, .damage = 2, .energyUsage = 1};

//-------------------------------------//

void drawFPS(uint16_t fps)
{
    char buffer[12];
	sprintf(buffer, "FPS: %i", fps);
	glDrawText(buffer, 3, 3, 0xFFFFFF);
}

void calcFrame(uint32_t ticks)
{
    #ifdef DEBUG
    if(counterEnabled)
    {
        counterTime += ticks;
        counterFrames++;
    }
    #endif

    #ifdef DEBUG
    if(keyUp(Z))
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
    }
    #endif
    
    if(state == TRADING)
    {
        if(keyUp(U))
        {
            moveTradeCursor(-1);
        }
        else if(keyUp(D))
        {
            moveTradeCursor(1);
        }
        else if(keyUp(L))
        {
            transferCargo(&playerShip.hold, &stationHold, getTradeCursor(), &starSystem.info);
        }
        else if(keyUp(R))
        {
            transferCargo(&stationHold, &playerShip.hold, getTradeCursor(), &starSystem.info);
        }
        else if(keyUp(B))
        {
            state = STATION;
        }
    }
    else if(state == MAP)
    {
        int8_t dirX = 0;
        int8_t dirY = 0;
        if(keyUp(U))
        {
            dirY = 1;
        }
        else if(keyUp(D))
        {
            dirY = -1;
        }
        if(keyUp(L))
        {
            dirX = -1;
        }
        else if(keyUp(R))
        {
            dirX = 1;
        }

        moveMapCursor(dirX, dirY);
        if(keyUp(S))
        {
            state = SPACE;
        }
        else if(keyUp(A))
        {
            switchSystem(getMapCursor(), &starSystem);
            state = SPACE;
        }
    }
    else
    {
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

        calcShip(&playerShip, &starSystem, ticks);
        setCameraPos(playerShip.position);
        setCameraRot(playerShip.rotation);

        if(keyUp(A))
        {
            fireWeapons(&playerShip, npcShips, 1);
        }

        calcUniverse(&state, &targetState, &starSystem, &playerShip, npcShips, ticks);

        if(keyUp(S) && state == SPACE)
        {
            state = MAP;
        }
    }
}

void drawFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);

    drawCamera();

    drawUniverse(&state, &starSystem, npcShips);

    drawFPS(fps);

    setOrtho();
    switch(state)
    {
        case SPACE:
        case STATION:
        {
            drawUI(state, &playerShip, npcShips, starSystem.station.position);
            break;
        }
        case TRADING:
        {
            drawTradingUI(&playerShip.hold, &stationHold, &starSystem.info);
            break;
        }
        case MAP:
        {
            drawMap(getSystemSeeds());
            break;
        }
    }
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
    initUniverse(&starSystem);
    initShip();
    createStationHold(&stationHold);

    //Temporary (TODO: REMOVE)
    playerShip.type = &test;
    playerShip.position.x = 150;
    playerShip.position.z = 100;
    playerShip.hold.money = 1000;
    playerShip.hold.size = 25;
    playerShip.weapon.type = &testWeapon;
    npcShips[0].type = &test;
    npcShips[0].position.x = 120;
    npcShips[0].position.z = 80;
    npcShips[0].weapon.type = &testWeapon;

    //Run main loop
	uint32_t tNow = SDL_GetTicks();
	uint32_t tLastFrame = tNow;
    uint16_t ticks = 0;
    while(running)
    {
		tNow = SDL_GetTicks();
        ticks = tNow - tLastFrame;
        
        running = handleInput();

        calcFrame(ticks);
        drawFrame();

        #ifdef LIMIT_FPS
		if((1000 / MAX_FPS) > (SDL_GetTicks() - tNow))
        {
			SDL_Delay((1000 / MAX_FPS) - (SDL_GetTicks() - tNow)); //Yay stable framerate!
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