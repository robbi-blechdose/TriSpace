#include <SDL.h>

#include "engine/video.h"
#include "engine/includes/3dMath.h"
#include "engine/input.h"
#include "engine/camera.h"
#include "engine/effects.h"
#include "engine/savegame.h"
#include "engine/audio.h"

#include "ship.h"
#include "ship_collisions.h"
#include "universe/universe.h"
#include "universe/starsystem.h"
#include "contracts.h"
#include "spacedust.h"
#include "autodocking.h"
#include "equipment.h"
#include "player.h"
#include "universe/asteroids.h"

#include "ui/popup.h"
#include "ui/ui.h"

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

Player player;
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

        savePlayer(&player);

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
        if((version / 100) != (SAVE_VERSION / 100)) //Check major version for save compatability
        {
            closeSave();
            return false;
        }

        loadPlayer(&player);

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
    return false;
}

void newGame()
{
    //Initialize player
    player.wantedLevel = 0;
    player.ship = (Ship) {.type = 0,
                            .position = (vec3) {150, 0, 100},
                            .weapon.type = 0,
                            .shields = 2,
                            .energy = 2};

    #ifdef DEBUG
    player.hold.money = 50000;
    #else
    player.hold.money = 150;
    #endif
    player.hold.size = CARGO_HOLD_SIZE_NORM;
    player.hasAutodock = false;
    player.hasFuelScoops = false;
    player.fuel = 3.5f;
    //Initialize system
    currentSystem[0] = 0;
    currentSystem[1] = 0;
    initSystem(currentSystem, &starSystem, npcShips);
    setInitialSpawnPos(player.ship.position);
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
        accelerateShip(&player.ship, 1, ticks);
    }
    else if(keyPressed(B_Y))
    {
        accelerateShip(&player.ship, -1, ticks);
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
    steerShip(&player.ship, dirX, dirY, ticks);
}

void calcSpace(uint32_t ticks)
{
    if(!autodock.active)
    {
        calcShipControl(ticks);

        if(keyUp(B_SELECT) && player.hasAutodock)
        {
            preCalcAutodockShip(&autodock, &player.ship, &starSystem);
            if(autodock.active)
            {
                playMusic(MUSIC_DOCKING, 250);
            }
        }
    }
    else
    {
        calcAutodockShip(&autodock, &player.ship, ticks);

        if(keyUp(B_SELECT))
        {
            autodock.active = 0;
        }

        if(!autodock.active)
        {
            playMusic(MUSIC_MAIN, 250);
        }
    }

    if(player.hasFuelScoops &&
        hasSunFuelDistance(&starSystem, &player.ship.position) && player.fuel < MAX_FUEL)
    {
        player.fuelScoopsActive = true;
        player.fuel += 0.5f * ticks / 1000.0f;
    }
    else
    {
        player.fuelScoopsActive = false;
    }

    calcShip(&player.ship, checkStarSystemCollision(&player.ship, &starSystem), ticks);
    setCameraPos(player.ship.position);
    setCameraRot(player.ship.rotation);
    calcSpacedust(&player.ship, ticks);

    if(keyUp(B_A))
    {
        if(fireWeapons(&player.ship))
        {
            if(!checkWeaponsShipHit(&player.ship, npcShips, MAX_NPC_SHIPS, DAMAGE_SOURCE_PLAYER))
            {
                checkWeaponsAsteroidHit(&player);
            }
        }
    }

    calcUniverse(&state, &starSystem, &player, npcShips, ticks);
    if(shipIsDestroyed(&player.ship))
    {
        vec3 effectPos = scalev3(2.5f, anglesToDirection(&player.ship.rotation));
        effectPos = addv3(player.ship.position, effectPos);
        createEffect(effectPos, EXPLOSION);
        state = GAME_OVER;
    }

    if(keyUp(B_START))
    {
        state = MAP;
    }
}

void calcContracts()
{
    if(checkClosePopup())
    {
        return;
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
            if(activateContract(&stationContracts[contractCursor], &player.hold))
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
            if(checkContract(&currentContract, &player.hold, currentSystem, npcShips))
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
}

void calcFrame(uint32_t ticks)
{
    switch(state)
    {
        case SPACE:
        {
            calcSpace(ticks);
            break;
        }
        case STATION:
        {
            calcShipControl(ticks);

            calcShip(&player.ship, checkStationCollision(&player.ship), ticks);
            setCameraPos(player.ship.position);
            setCameraRot(player.ship.rotation);

            calcUniverse(&state, &starSystem, &player, npcShips, ticks);
            break;
        }
        case HYPERSPACE:
        {
            if(currentSystem[0] != uiMapCursor[0] || currentSystem[1] != uiMapCursor[1])
            {
                player.ship.speed += (500.0f * ticks) / 1000.0f;
                if(player.ship.speed > 500)
                {
                    switchSystem(currentSystem, uiMapCursor, &starSystem, npcShips);
                    player.ship.position = getRandomFreePos(&starSystem, 20);
                    setInitialSpawnPos(player.ship.position);
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
                player.ship.speed -= (500.0f * ticks) / 1000.0f;
                if(player.ship.speed <= 0)
                {
                    player.ship.speed = 0;
                    state = SPACE;
                }
            }
            calcShip(&player.ship, 0, ticks);
            setCameraPos(player.ship.position);
            setCameraRot(player.ship.rotation);
            calcSpacedust(&player.ship, ticks);
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
                transferCargo(&player.hold, &stationHold, uiTradeCursor, &starSystem.info, 1);
            }
            else if(keyUp(B_RIGHT))
            {
                transferCargo(&stationHold, &player.hold, uiTradeCursor, &starSystem.info, 0);
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
                buyEquipment(&player, uiEquipCursor);
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
            calcContracts();
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
                if(player.fuel >= distance)
                {
                    state = HYPERSPACE;
                    player.fuel -= distance;
                    player.ship.turnSpeedX = 0;
                    player.ship.turnSpeedY = 0;
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
                player.ship.speed = 0;
                player.ship.turnSpeedX = 0;
                player.ship.turnSpeedY = 0;
                for(uint8_t i = 0; i < NUM_CARGO_TYPES; i++)
                {
                    player.hold.cargo[i] = 0;
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
            drawUI(state, &player, npcShips, starSystem.station.position, player.hasAutodock && isAutodockPossible(&player.ship, &starSystem));
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
            drawTradingUI(uiTradeCursor, &player.hold, &stationHold, &starSystem.info);
            break;
        }
        case EQUIP:
        {
            drawEquipUI(uiEquipCursor, &player);
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
            drawMap(uiMapCursor, currentSystem, player.fuel);
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
            drawUI(state, &player, npcShips, starSystem.station.position, isAutodockPossible(&player.ship, &starSystem));
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
    initPopup();

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