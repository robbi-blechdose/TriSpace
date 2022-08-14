#include <SDL.h>
#define CHAD_API_IMPL
#include "zbuffer.h"
#include "engine/includes/3dMath.h"
#include "engine/input.h"
#include "engine/camera.h"
#include "engine/view.h"
#include "engine/effects.h"
#include "engine/savegame.h"
#include "engine/audio.h"

#include "ui.h"
#include "ship.h"
#include "ship_collisions.h"
#include "universe/universe.h"
#include "universe/starsystem.h"
#include "contracts.h"
#include "spacedust.h"
#include "autodocking.h"
#include "equipment.h"

//Compile with debug functionality
//#define DEBUG

#define WINX 240
#define WINY 240
#define WINY_3D (WINY - 70)
#define MAX_FPS 50
#define LIMIT_FPS

#define SAVE_VERSION 400

SDL_Surface* screen;
ZBuffer* frameBuffer = NULL;

#ifdef DEBUG
uint16_t fps;
uint8_t counterEnabled = 0;
uint32_t counterFrames = 0;
uint32_t counterTime = 0;
uint16_t counterResult = 0;
#endif

#define MUSIC_DOCKING 0
#define MUSIC_MAIN    1

bool running = true;

//---------- Main game stuff ----------//
State state;

uint8_t currentSystem[2];
StarSystem starSystem;

uint8_t completedContracts[UNIVERSE_SIZE][UNIVERSE_SIZE];

CargoHold stationHold;
Contract stationContracts[MAX_STATION_CONTRACTS];
uint8_t numStationContracts;

Ship playerShip;
Ship npcShips[MAX_NPC_SHIPS];

Contract currentContract;

AutodockData autodock;

uint8_t uiSaveLoadCursor;
uint8_t uiTradeCursor;
uint8_t uiEquipCursor;
uint8_t uiContractCursor;
uint8_t uiMapCursor[2];
uint8_t uiTitleCursor;

//-------------------------------------//

#ifdef DEBUG
void drawFPS(uint16_t fps)
{
    char buffer[12];
	sprintf(buffer, "FPS: %i", fps);
	glDrawText(buffer, 2, 2, 0xFFFFFF);
	sprintf(buffer, "AVG: %i", counterResult);
	glDrawText(buffer, 2, 10, 0xFFFFFF);
}
#endif

uint8_t saveGame()
{
    openSave(".trispace", "game.sav", 1);
    uint16_t version = SAVE_VERSION;
    writeElement(&version, sizeof(version));
    writeElement(&playerShip.type, sizeof(playerShip.type));
    writeElement(&playerShip.hold, sizeof(playerShip.hold));
    writeElement(&playerShip.fuel, sizeof(playerShip.fuel));
    writeElement(&playerShip.hasAutodock, sizeof(playerShip.hasAutodock));
    writeElement(&playerShip.weapon.type, sizeof(playerShip.weapon.type));
    writeElement(&currentSystem[0], sizeof(uint8_t));
    writeElement(&currentSystem[1], sizeof(uint8_t));
    writeElement(&currentContract, sizeof(currentContract));
    writeElement(&completedContracts, sizeof(completedContracts));
    closeSave();
    return 1;
}

uint8_t loadGame()
{
    if(openSave(".trispace", "game.sav", 0))
    {
        uint16_t version = 0;
        readElement(&version, sizeof(version));
        if((version / 100) == (SAVE_VERSION / 100)) //Check major version for save compatability
        {
            readElement(&playerShip.type, sizeof(playerShip.type));
            readElement(&playerShip.hold, sizeof(playerShip.hold));
            readElement(&playerShip.fuel, sizeof(playerShip.fuel));
            readElement(&playerShip.hasAutodock, sizeof(playerShip.hasAutodock));
            readElement(&playerShip.weapon.type, sizeof(playerShip.weapon.type));
            uint8_t savedSystem[2];
            readElement(&savedSystem[0], sizeof(uint8_t));
            readElement(&savedSystem[1], sizeof(uint8_t));
            deleteStarSystem(&starSystem);
            initSystem(currentSystem, &starSystem, npcShips);
            readElement(&currentContract, sizeof(currentContract));
            readElement(&completedContracts, sizeof(completedContracts));
            return 1;
        }
        closeSave();
    }
    return 0;
}

void newGame()
{
    //Initialize player ship
    playerShip.type = 0;
    playerShip.position.x = 150;
    playerShip.position.y = 0;
    playerShip.position.z = 100;
    #ifdef DEBUG
    playerShip.hold.money = 50000;
    #else
    playerShip.hold.money = 150;
    #endif
    playerShip.hold.size = CARGO_HOLD_SIZE_NORM;
    playerShip.weapon.type = 0;
    playerShip.hasAutodock = 0;
    playerShip.fuel = 35;
    playerShip.shields = 2;
    playerShip.energy = 2;
    //Initialize system
    currentSystem[0] = 0;
    currentSystem[1] = 0;
    initSystem(currentSystem, &starSystem, npcShips);
    setInitialSpawnPos(playerShip.position);
}

uint8_t checkClosePopup()
{
    if(isPopupOpen())
    {
        if(keyUp(B_A) || keyUp(B_B))
        {
            closePopup();
            return 1;
        }
    }
    return 0;
}

void calcShipControl(uint32_t ticks)
{
    if(keyPressed(B_X))
    {
        accelerateShip(&playerShip, 1, ticks);
    }
    else if(keyPressed(B_Y))
    {
        accelerateShip(&playerShip, -1, ticks);
    }
    int8_t dirX = 0;
    int8_t dirY = 0;
    if(keyPressed(B_UP))
    {
        dirX = 1;
    }
    else if(keyPressed(B_DOWN))
    {
        dirX = -1;
    }
    if(keyPressed(B_LEFT))
    {
        dirY = -1;
    }
    else if(keyPressed(B_RIGHT))
    {
        dirY = 1;
    }
    steerShip(&playerShip, dirX, dirY, ticks);
}

void calcFrame(uint32_t ticks)
{
    #ifdef DEBUG
    if(counterEnabled)
    {
        counterTime += ticks;
        counterFrames++;
    }

    if(keyUp(B_SELECT))
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
        {
            if(!autodock.active)
            {
                calcShipControl(ticks);

                if(keyUp(B_SELECT) && playerShip.hasAutodock)
                {
                    preCalcAutodockShip(&autodock, &playerShip, &starSystem);
                    if(autodock.active)
                    {
                        playMusic(MUSIC_DOCKING, 250);
                    }
                }
            }
            else
            {
                calcAutodockShip(&autodock, &playerShip, ticks);

                if(keyUp(B_SELECT))
                {
                    autodock.active = 0;
                }

                if(!autodock.active)
                {
                    playMusic(MUSIC_MAIN, 250);
                }
            }

            calcShip(&playerShip, checkStarSystemCollision(&playerShip, &starSystem), ticks);
            setCameraPos(playerShip.position);
            setCameraRot(playerShip.rotation);
            calcSpacedust(&playerShip, ticks);

            if(keyUp(B_A))
            {
                fireWeapons(&playerShip, npcShips, MAX_NPC_SHIPS, DAMAGE_SOURCE_PLAYER);
            }

            calcUniverse(&state, &starSystem, &playerShip, npcShips, ticks);
            if(shipIsDestroyed(&playerShip))
            {
                vec3 effectPos = scalev3(2.5f, anglesToDirection(&playerShip.rotation));
                effectPos = addv3(playerShip.position, effectPos);
                createEffect(effectPos, EXPLOSION);
                state = GAME_OVER;
            }

            if(keyUp(B_START))
            {
                state = MAP;
            }
            break;
        }
        case STATION:
        {
            calcShipControl(ticks);

            calcShip(&playerShip, checkStationCollision(&playerShip), ticks);
            setCameraPos(playerShip.position);
            setCameraRot(playerShip.rotation);

            calcUniverse(&state, &starSystem, &playerShip, npcShips, ticks);
            break;
        }
        case HYPERSPACE:
        {
            if(currentSystem[0] != uiMapCursor[0] || currentSystem[1] != uiMapCursor[1])
            {
                playerShip.speed += (500.0f * ticks) / 1000.0f;
                if(playerShip.speed > 500)
                {
                    switchSystem(currentSystem, uiMapCursor, &starSystem, npcShips);
                    playerShip.position = getRandomFreePos(&starSystem, 20);
                    setInitialSpawnPos(playerShip.position);
                    //Generate contracts for this system
                    generateContractsForSystem(stationContracts, &numStationContracts, &starSystem.info, currentSystem, completedContracts);
                    //Generate new station cargo hold for this system
                    createStationHold(&stationHold);
                    //Set up the current contract (if necessary)
                    contractStarSystemSetup(&currentContract, npcShips, currentSystem, &starSystem);
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
            calcSpacedust(&playerShip, ticks);
            break;
        }
        case SAVELOAD:
        {
            if(checkClosePopup())
            {
                break;
            }

            if(keyUp(B_UP) || keyUp(B_DOWN))
            {
                if(uiSaveLoadCursor)
                {
                    uiSaveLoadCursor = 0;
                }
                else
                {
                    uiSaveLoadCursor = 1;
                }
            }
            else if(keyUp(B_A))
            {
                if(uiSaveLoadCursor == 0)
                {
                    uint8_t ok = saveGame();
                    if(ok)
                    {
                        createPopup(POPUP_CHECKMARK, "Game saved.");
                    }
                    else
                    {
                        createPopup(POPUP_ATTENTION, "Failed to save.");
                    }
                }
                else
                {
                    uint8_t ok = loadGame();
                    if(ok)
                    {
                        createPopup(POPUP_CHECKMARK, "Game loaded.");
                    }
                    else
                    {
                        createPopup(POPUP_ATTENTION, "Failed to load.");
                    }
                }
            }
            else if(keyUp(B_B))
            {
                state = STATION;
            }
            else if(keyUp(B_TR))
            {
                state = TRADING;
            }
            break;
        }
        case TRADING:
        {
            if(keyUp(B_UP))
            {
                moveCursorUp(&uiTradeCursor, NUM_CARGO_TYPES - 1);
            }
            else if(keyUp(B_DOWN))
            {
                uiTradeCursor = (uiTradeCursor + 1) % NUM_CARGO_TYPES;
            }
            else if(keyUp(B_LEFT))
            {
                transferCargo(&playerShip.hold, &stationHold, uiTradeCursor, &starSystem.info, 1);
            }
            else if(keyUp(B_RIGHT))
            {
                transferCargo(&stationHold, &playerShip.hold, uiTradeCursor, &starSystem.info, 0);
            }
            else if(keyUp(B_B))
            {
                state = STATION;
            }
            else if(keyUp(B_TL))
            {
                state = SAVELOAD;
            }
            else if(keyUp(B_TR))
            {
                state = EQUIP;
            }
            break;
        }
        case EQUIP:
        {
            if(keyUp(B_UP))
            {
                moveCursorUp(&uiEquipCursor, NUM_EQUIPMENT_TYPES - 1);
            }
            else if(keyUp(B_DOWN))
            {
                uiEquipCursor = (uiEquipCursor + 1) % NUM_EQUIPMENT_TYPES;
            }
            else if(keyUp(B_A))
            {
                buyEquipment(&playerShip, uiEquipCursor);
            }
            else if(keyUp(B_B))
            {
                state = STATION;
            }
            else if(keyUp(B_TL))
            {
                state = TRADING;
            }
            else if(keyUp(B_TR))
            {
                state = CONTRACTS;
            }
            break;
        }
        case CONTRACTS:
        {
            if(checkClosePopup())
            {
                break;
            }

            if(keyUp(B_UP))
            {
                moveCursorUp(&uiContractCursor, numStationContracts - 1);
            }
            else if(keyUp(B_DOWN))
            {
                moveCursorDown(&uiContractCursor, numStationContracts - 1);
            }
            else if(keyUp(B_A))
            {
                if(currentContract.type == CONTRACT_TYPE_NULL)
                {
                    uint8_t contractCursor = uiContractCursor;
                    if(activateContract(&stationContracts[contractCursor], &playerShip.hold))
                    {
                        currentContract = stationContracts[contractCursor];

                        //Shift contracts forward
                        for(uint8_t i = contractCursor; i < numStationContracts - 1; i++)
                        {
                            stationContracts[i] = stationContracts[i + 1];
                        }
                        stationContracts[numStationContracts - 1].type = CONTRACT_TYPE_NULL;
                        numStationContracts--;
                    }
                    else
                    {
                        createPopup(POPUP_ATTENTION, "Cannot activate\ncontract.\nMake sure your\ncargo bay has\nspace!");
                    }
                }
                else
                {
                    if(checkContract(&currentContract, &playerShip.hold, currentSystem, npcShips))
                    {
                        uint8_t buffer[64];
                        sprintf(buffer, "Contract done.\n%d credits\nhave been\ntransferred.", currentContract.pay);
                        createPopup(POPUP_CHECKMARK, buffer);
                        completedContracts[currentSystem[0]][currentSystem[1]]++;
                        currentContract.type = CONTRACT_TYPE_NULL;
                        uiContractCursor = 0;
                    }
                }
            }
            else if(keyUp(B_B))
            {
                state = STATION;
            }
            else if(keyUp(B_TL))
            {
                state = EQUIP;
            }
            break;
        }
        case MAP:
        {
            if(keyUp(B_UP))
            {
                if(uiMapCursor[1] < UNIVERSE_SIZE - 1)
                {
                    uiMapCursor[1]++;
                }
            }
            else if(keyUp(B_DOWN))
            {
                if(uiMapCursor[1] > 0)
                {
                    uiMapCursor[1]--;
                }
            }
            if(keyUp(B_LEFT))
            {
                if(uiMapCursor[0] > 0)
                {
                    uiMapCursor[0]--;
                }
            }
            else if(keyUp(B_RIGHT))
            {
                if(uiMapCursor[0] < UNIVERSE_SIZE - 1)
                {
                    uiMapCursor[0]++;
                }
            }

            if(keyUp(B_START))
            {
                state = SPACE;
            }
            else if(keyUp(B_A))
            {
                float distance = getDistanceToSystem(currentSystem, uiMapCursor);
                if(playerShip.fuel >= distance * 10)
                {
                    state = HYPERSPACE;
                    playerShip.fuel -= distance * 10;
                    playerShip.turnSpeedX = 0;
                    playerShip.turnSpeedY = 0;
                }
            }
            break;
        }
        case TITLE:
        {
            if(checkClosePopup())
            {
                break;
            }

            if(keyUp(B_UP) || keyUp(B_DOWN))
            {
                if(uiTitleCursor)
                {
                    uiTitleCursor = 0;
                }
                else
                {
                    uiTitleCursor = 1;
                }
            }
            else if(keyUp(B_A) || keyUp(B_START))
            {
                if(uiTitleCursor == 0)
                {
                    newGame();
                    state = SPACE;
                }
                else
                {
                    if(loadGame())
                    {
                        state = STATION;
                    }
                    else
                    {
                        createPopup(POPUP_ATTENTION, "Failed to load.");
                    }
                }
            }
            break;
        }
        case GAME_OVER:
        {
            calcEffects(ticks);

            if(keyUp(B_A) || keyUp(B_START))
            {
                state = TITLE;
                //Clear player ship fields
                playerShip.speed = 0;
                playerShip.turnSpeedX = 0;
                playerShip.turnSpeedY = 0;
                for(uint8_t i = 0; i < NUM_CARGO_TYPES; i++)
                {
                    playerShip.hold.cargo[i] = 0;
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
    switch(state)
    {
        case SPACE:
        case HYPERSPACE:
        case STATION:
        case GAME_OVER:
        {
            drawUniverse(&state, &starSystem, npcShips);
            break;
        }
    }
    if(state == HYPERSPACE)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    if(state == SPACE || state == HYPERSPACE || state == GAME_OVER)
    {
        drawSpacedust();
    }

    #ifdef DEBUG
    drawFPS(fps);
    #endif

    //GUI drawing
    setOrtho();
    switch(state)
    {
        case SPACE:
        case STATION:
        case HYPERSPACE:
        {
            drawUI(state, &playerShip, npcShips, starSystem.station.position, playerShip.hasAutodock && isAutodockPossible(&playerShip, &starSystem));
            break;
        }
        case SAVELOAD:
        {
            drawSaveLoadUI(uiSaveLoadCursor);
            drawPopupIfActive();
            break;
        }
        case TRADING:
        {
            drawTradingUI(uiTradeCursor, &playerShip.hold, &stationHold, &starSystem.info);
            break;
        }
        case EQUIP:
        {
            drawEquipUI(uiEquipCursor, &playerShip);
            break;
        }
        case CONTRACTS:
        {
            drawContractUI(uiContractCursor, &currentContract, stationContracts, numStationContracts);
            drawPopupIfActive();
            break;
        }
        case MAP:
        {
            drawMap(uiMapCursor, currentSystem, playerShip.fuel / 10.0f);
            break;
        }
        case TITLE:
        {
            drawTitleScreen(uiTitleCursor);
            drawPopupIfActive();
            break;
        }
        case GAME_OVER:
        {
            //Keep drawing game UI
            drawUI(state, &playerShip, npcShips, starSystem.station.position, isAutodockPossible(&playerShip, &starSystem));
            drawGameOverScreen();
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
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
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
	glClearColor(0, 0, 0.1f, 0);
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

    initAudio(MIX_MAX_VOLUME, 2, 2);
    loadMusic(MUSIC_DOCKING, "res/music/Blue_Danube.ogg");
    loadMusic(MUSIC_MAIN, "res/music/menuLoops_rock.ogg");

    //Init UI variables to zero
    uiSaveLoadCursor = 0;
    uiTradeCursor = 0;
    uiEquipCursor = 0;
    uiContractCursor = 0;
    uiMapCursor[0] = 0;
    uiMapCursor[1] = 0;
    uiTitleCursor = 0;

    //Initialize main systems
    initUI();
    initUniverse(currentSystem, &starSystem, npcShips);
    initShip();
    initSpacedust();
    createStationHold(&stationHold);
    state = TITLE;
    currentContract.type = CONTRACT_TYPE_NULL;
    generateContractsForSystem(stationContracts, &numStationContracts, &starSystem.info, currentSystem, completedContracts);

    playMusic(MUSIC_MAIN, 0);

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
		while((1000 / MAX_FPS) > (SDL_GetTicks() - tNow + 1))
        {
			//SDL_Delay((1000 / MAX_FPS) - (SDL_GetTicks() - tNow)); //Yay stable framerate!
			SDL_Delay(1); //Yay stable framerate!
		}
        #endif
        #ifdef DEBUG
		fps = 1000.0f / (float)(tNow - tLastFrame);
        #endif
		tLastFrame = tNow;
    }

    //Cleanup
    quitAudio();
    
	ZB_close(frameBuffer);
	glClose();

    SDL_Quit();

	return 0;
}