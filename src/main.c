#include <SDL.h>
#define CHAD_API_IMPL
#include "zbuffer.h"
#include "engine/includes/3dMath.h"
#include "engine/input.h"
#include "engine/camera.h"
#include "engine/view.h"
#include "engine/effects.h"
#include "engine/savegame.h"

#include "ui.h"
#include "ship.h"
#include "ship_collisions.h"
#include "universe/universe.h"
#include "universe/starsystem.h"
#include "contracts.h"

//Compile with debug functionality
#define DEBUG

#define WINX 240
#define WINY 240
#define WINY_3D (WINY - 70)
#define MAX_FPS 50
//#define LIMIT_FPS

#define SAVE_VERSION 101

SDL_Surface* screen;
ZBuffer* frameBuffer = NULL;
SDL_Event event;
uint16_t fps;

#ifdef DEBUG
uint8_t counterEnabled = 0;
uint32_t counterFrames = 0;
uint32_t counterTime = 0;
uint16_t counterResult = 0;
#endif

uint8_t running = 1;

//---------- Main game stuff ----------//
State state;

StarSystem starSystem;
vec3 jumpStart;

CargoHold stationHold;
Contract stationContracts[4];
uint8_t numStationContracts;

Ship playerShip;
Ship npcShips[MAX_NPC_SHIPS];

Contract currentContract;

//-------------------------------------//

void drawFPS(uint16_t fps)
{
    char buffer[12];
	sprintf(buffer, "FPS: %i", fps);
	glDrawText(buffer, 2, 2, 0xFFFFFF);
	sprintf(buffer, "FPS: %i", counterResult);
	glDrawText(buffer, 2, 10, 0xFFFFFF);
}

uint8_t saveGame()
{
    openSave(".trispace", "game.sav", 1);
    uint8_t version = SAVE_VERSION;
    writeElement(&version, sizeof(version));
    writeElement(&playerShip.type, sizeof(playerShip.type));
    writeElement(&playerShip.hold, sizeof(playerShip.hold));
    writeElement(&playerShip.fuel, sizeof(playerShip.fuel));
    uint32_t currentSystem = getCurrentSystem();
    writeElement(&currentSystem, sizeof(uint32_t));
    writeElement(&currentContract, sizeof(currentContract));
    closeSave();
    return 1;
}

uint8_t loadGame()
{
    openSave(".trispace", "game.sav", 0);
    uint8_t version = 0;
    readElement(&version, sizeof(version));
    if((version / 100) == (SAVE_VERSION / 100)) //Check major version for save compatability
    {
        readElement(&playerShip.type, sizeof(playerShip.type));
        readElement(&playerShip.hold, sizeof(playerShip.hold));
        readElement(&playerShip.fuel, sizeof(playerShip.fuel));
        uint32_t savedSystem;
        readElement(&savedSystem, sizeof(uint32_t));
        switchSystem(savedSystem, &starSystem, npcShips);
        readElement(&currentContract, sizeof(currentContract));
        return 1;
    }
    else
    {
        return 0;
    }
    closeSave();
}

void calcFrame(uint32_t ticks)
{
    #ifdef DEBUG
    if(counterEnabled)
    {
        counterTime += ticks;
        counterFrames++;
    }

    if(keyUp(K))
    {
        counterEnabled = !counterEnabled;
        if(!counterEnabled)
        {
            counterResult = ((float) counterFrames / counterTime) * 1000.0f;
            printf("%d frames in %d ms. %d fps. %f ms/frame.\n", counterFrames, counterTime,
                                                        counterResult,
                                                        (float) counterTime / counterFrames);
            counterTime = 0;
            counterFrames = 0;
        }
    }
    #endif

    switch(state)
    {
        case SPACE:
        case STATION:
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

            uint8_t collided = 0;
            if(state == SPACE)
            {
                collided = checkStarSystemCollision(&playerShip, &starSystem);
            }
            else
            {
                collided = checkStationCollision(&playerShip);
            }

            calcShip(&playerShip, collided, ticks);
            setCameraPos(playerShip.position);
            setCameraRot(playerShip.rotation);

            if(keyUp(A))
            {
                fireWeapons(&playerShip, npcShips, MAX_NPC_SHIPS);
            }

            calcUniverse(&state, &starSystem, &playerShip, npcShips, ticks);

            if(keyUp(S) && state == SPACE)
            {
                state = MAP;
            }
            break;
        }
        case HYPERSPACE:
        {
            if(getCurrentSystem() != getMapCursor())
            {
                playerShip.speed += (500.0f * ticks) / 1000.0f;
                if(playerShip.speed > 500)
                {
                    switchSystem(getMapCursor(), &starSystem, npcShips);
                    playerShip.position = jumpStart;
                }
            }
            else
            {
                playerShip.speed -= (500.0f * ticks) / 1000.0f;
                if(playerShip.speed <= 0)
                {
                    playerShip.speed = 0;
                    state = SPACE;
                }
            }
            calcShip(&playerShip, 0, ticks);
            setCameraPos(playerShip.position);
            setCameraRot(playerShip.rotation);
            break;
        }
        case SAVELOAD:
        {
            if(keyUp(U) || keyUp(D))
            {
                toggleSaveLoadCursor();
            }
            else if(keyUp(A))
            {
                if(getSaveLoadCursor() == 0)
                {
                    saveGame();
                    //TODO: Read return value and display success or error message
                }
                else
                {
                    loadGame();
                    //TODO: Read return value and display success or error message
                }
            }
            else if(keyUp(B))
            {
                state = STATION;
            }
            else if(keyUp(N))
            {
                state = TRADING;
            }
            break;
        }
        case TRADING:
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
            else if(keyUp(M))
            {
                state = SAVELOAD;
            }
            else if(keyUp(N))
            {
                state = EQUIP;
            }
            break;
        }
        case EQUIP:
        {
            if(keyUp(U))
            {
                moveEquipCursor(-1);
            }
            else if(keyUp(D))
            {
                moveEquipCursor(1);
            }
            else if(keyUp(A))
            {
                switch(getEquipCursor())
                {
                    case EQUIP_FUEL:
                    {
                        if(playerShip.fuel < MAX_FUEL && playerShip.hold.money >= 2)
                        {
                            playerShip.fuel += 5;
                            playerShip.hold.money -= 2;
                            if(playerShip.fuel > MAX_FUEL)
                            {
                                playerShip.fuel = MAX_FUEL;
                            }
                        }
                        break;
                    }
                    case EQUIP_HOLD30:
                    {
                        if(playerShip.hold.money >= 1000)
                        {
                            playerShip.hold.size = 30;
                            playerShip.hold.money-= 1000;
                        }
                        break;
                    }
                }
            }
            else if(keyUp(B))
            {
                state = STATION;
            }
            else if(keyUp(M))
            {
                state = TRADING;
            }
            else if(keyUp(N))
            {
                state = CONTRACTS;
            }
            break;
        }
        case CONTRACTS:
        {
            if(keyUp(U))
            {
                moveContractCursor(-1, numStationContracts);
            }
            else if(keyUp(D))
            {
                moveContractCursor(1, numStationContracts);
            }
            else if(keyUp(A))
            {
                if(currentContract.type == CONTRACT_TYPE_NULL)
                {
                    currentContract = stationContracts[getContractCursor()];
                    //Remove first contract and shift the rest
                    for(uint8_t i = 0; i < numStationContracts - 1; i++)
                    {
                        stationContracts[i] = stationContracts[i + 1];
                    }
                    //TODO: Mark as active somehow?
                }
            }
            else if(keyUp(B))
            {
                state = STATION;
            }
            else if(keyUp(M))
            {
                state = EQUIP;
            }
            break;
        }
        case MAP:
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
                //TODO: replace with logic using the actual distance between systems
                if(playerShip.fuel >= 20)
                {
                    jumpStart = playerShip.position;
                    state = HYPERSPACE;
                    playerShip.fuel -= 20;
                }
            }
            break;
        }
        case TITLE:
        {
            if(keyUp(U) || keyUp(D))
            {
                toggleTitleCursor();
            }
            else if(keyUp(A) || keyUp(S))
            {
                if(getTitleCursor() == 0)
                {
                    //Init new game
                    //Temporary (TODO: REMOVE)
                    playerShip.type = 0;
                    playerShip.position.x = 150;
                    playerShip.position.z = 100;
                    playerShip.hold.money = 1000;
                    playerShip.hold.size = 25;
                    playerShip.weapon.type = 0;
                    playerShip.fuel = 35;
                    state = SPACE;
                }
                else
                {
                    //Load game
                    loadGame();
                    state = STATION;
                }
            }
            break;
        }
    }
}

void drawFrame()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);

    drawCamera();

    if(state == HYPERSPACE)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    drawUniverse(&state, &starSystem, npcShips);
    if(state == HYPERSPACE)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    drawFPS(fps);

    setOrtho();
    switch(state)
    {
        case SPACE:
        case STATION:
        case HYPERSPACE:
        {
            drawUI(state, &playerShip, npcShips, starSystem.station.position, 0, playerShip.position);
            break;
        }
        case SAVELOAD:
        {
            drawSaveLoadUI();
            break;
        }
        case TRADING:
        {
            drawTradingUI(&playerShip.hold, &stationHold, &starSystem.info);
            break;
        }
        case EQUIP:
        {
            drawEquipUI(&playerShip);
            break;
        }
        case CONTRACTS:
        {
            drawContractUI(stationContracts, getSystemSeeds(), numStationContracts);
            break;
        }
        case MAP:
        {
            drawMap(getSystemSeeds());
            break;
        }
        case TITLE:
        {
            drawTitleScreen();
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

    //Initialize main systems
    initUI();
    initUniverse(&starSystem);
    initShip();
    createStationHold(&stationHold);
    state = TITLE;
    currentContract.type = CONTRACT_TYPE_NULL;

    //TODO: Remove test
    numStationContracts = 4;
    for(uint8_t i = 0; i < numStationContracts; i++)
    {
        stationContracts[i] = generateContract(0, &starSystem.info);
    }

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