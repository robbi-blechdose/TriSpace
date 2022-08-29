#include <SDL.h>

#include "engine/video.h"
#include "engine/includes/3dMath.h"
#include "engine/input.h"
#include "engine/camera.h"
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

#define WINY_3D (WINY - 70)
#define MAX_FPS 50

#ifdef DEBUG
uint16_t fps;
#define FPS_WINDOW 250
uint32_t counterFrames = 0;
uint32_t counterTime = 0;
#endif

#define SAVE_VERSION 500

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

bool saveGame()
{
    if(openSave(".trispace", "game.sav", 1))
    {
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
        return true;
    }
    return false;
}

bool loadGame()
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
            closeSave();
            return true;
        }
        closeSave();
    }
    return false;
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
    playerShip.fuel = 3.5f;
    playerShip.shields = 2;
    playerShip.energy = 2;
    //Initialize system
    currentSystem[0] = 0;
    currentSystem[1] = 0;
    initSystem(currentSystem, &starSystem, npcShips);
    setInitialSpawnPos(playerShip.position);
}

bool checkClosePopup()
{
    if(isPopupOpen())
    {
        if(keyUp(B_A) || keyUp(B_B))
        {
            closePopup();
            return true;
        }
    }
    return false;
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

            playerShip.fuelScoopsActive = hasSunFuelDistance(&starSystem, &playerShip.position);

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
                    if(saveGame())
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
                    if(loadGame())
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
                if(playerShip.fuel >= distance)
                {
                    state = HYPERSPACE;
                    playerShip.fuel -= distance;
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
	clearFrame();
    glEnable(GL_CULL_FACE);

    glViewport(0, 0, WINX, WINY_3D);
    setPerspective();

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
    glViewport(0, 0, WINX, WINY);
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
            drawMap(uiMapCursor, currentSystem, playerShip.fuel);
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

    flipFrame();
}

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);

    initVideo((vec4) {.d = {0, 0, 0.1f, 0}}, (vec4) {.d = {0, 0, WINX, WINY_3D}}, 70, 1, 512);

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

        #ifndef NO_FPS_LIMIT
		while((1000 / MAX_FPS) > (SDL_GetTicks() - tNow + 1))
        {
			//SDL_Delay((1000 / MAX_FPS) - (SDL_GetTicks() - tNow)); //Yay stable framerate!
			SDL_Delay(1); //Yay stable framerate!
		}
        #endif
        #ifdef DEBUG
		//fps = 1000.0f / (float)(tNow - tLastFrame);
        counterTime += ticks;
        counterFrames++;
        if(counterTime > FPS_WINDOW)
        {
            fps = ((float) counterFrames / counterTime) * 1000.0f;
            counterTime = 0;
            counterFrames = 0;
        }
        #endif
		tLastFrame = tNow;
    }

    //Cleanup
    quitAudio();
    
	quitVideo();

	return 0;
}