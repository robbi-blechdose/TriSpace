#include <SDL.h>

#include "engine/video.h"
#include "engine/includes/3dMath.h"
#include "engine/input.h"
#include "engine/camera.h"
#include "engine/effects.h"
#include "engine/savegame.h"
#include "engine/audio.h"

#include "universe/starsystem.h"
#include "universe/spacestation.h"
#include "universe/asteroids.h"
#include "universe/universe.h"
#include "universe/satellites.h"

#include "ship.h"
#include "ship_collisions.h"
#include "contracts.h"
#include "spacedust.h"
#include "autodocking.h"
#include "equipment.h"
#include "player.h"

#include "ui/popup.h"
#include "ui/ui.h"
#include "ui/starmap.h"
#include "ui/equipui.h"
#include "ui/titlescreen.h"
#include "ui/contractui.h"

#include "npcs/npc.h"
#include "npcs/ai.h"

#define WINY_3D (WINY - 70)
#define MAX_FPS 50

#ifdef DEBUG
uint16_t fps;
#define FPS_WINDOW 250
uint32_t counterFrames = 0;
uint32_t counterTime = 0;
#endif

typedef enum {
    NONE,
    //World states
    SPACE,
    STATION,
    //PLANET,
    HYPERSPACE,
    //GUI states
    SAVELOAD,
    TRADING,
    EQUIP,
    CONTRACTS,
    MAP,
    //Special states
    TITLE,
    GAME_OVER
} State;

#define SAVE_VERSION 60

#define MUSIC_DOCKING 0
#define MUSIC_MAIN    1

//---------- Main game stuff ----------//
bool running = true;

State state;

uint8_t currentSystem[2] = {0, 0};
StarSystem starSystem;

uint8_t completedContracts[UNIVERSE_SIZE][UNIVERSE_SIZE];

CargoHold stationHold;
Contract stationContracts[MAX_STATION_CONTRACTS];
uint8_t numStationContracts;

Player player;
Npc npcs[MAX_NPCS];

Contract currentContract = (Contract) {.type = CONTRACT_TYPE_NULL};

AutodockData autodock;

uint8_t uiSaveLoadCursor;
uint8_t uiTradeCursor;

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
        if(version / 10 != SAVE_VERSION / 10) //Check major version for save compatability
        {
            closeSave();
            return false;
        }

        player.ship.rotation = QUAT_INITIAL;
        loadPlayer(&player);

        uint8_t savedSystem[2];
        readElement(&savedSystem[0], sizeof(uint8_t));
        readElement(&savedSystem[1], sizeof(uint8_t));
        deleteStarSystem(&starSystem);
        initSystem(currentSystem, &starSystem, npcs);
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
                          .rotation = QUAT_INITIAL,
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
    player.fuel = MAX_FUEL;
    //Initialize system
    currentSystem[0] = 0;
    currentSystem[1] = 0;
    initSystem(currentSystem, &starSystem, npcs);
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
        dirX = -1;
    }
    else if(keyPressed(B_DOWN))
    {
        dirX = 1;
    }
    if(keyPressed(B_LEFT))
    {
        dirY = 1;
    }
    else if(keyPressed(B_RIGHT))
    {
        dirY = -1;
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
        player.fuel += 0.8f * ticks / 1000.0f;
    }
    else
    {
        player.fuelScoopsActive = false;
    }

    calcShip(&player.ship, ticks);
    if(checkStarSystemCollision(&player.ship, &starSystem))
    {
        player.ship.shields = -1;
    }
    setCameraPos(player.ship.position);
    setCameraRot(player.ship.rotation);
    calcSpacedust(&player.ship, ticks);

    if(keyUp(B_A))
    {
        if(fireWeapons(&player.ship))
        {
            Ship* npcShips[MAX_NPCS];
            for(uint8_t i = 0; i < MAX_NPCS; i++)
            {
                npcShips[i] = &npcs[i].ship;
            }
            if(!checkWeaponsShipHit(&player.ship, npcShips, MAX_NPCS, DAMAGE_SOURCE_PLAYER))
            {
                checkWeaponsAsteroidHit(&player);
            }
        }
    }

    if(hasSatellites())
    {
        checkVisitSatellite(&player.ship.position);
    }
    calcUniverseSpawnNPCShips(&starSystem, &player.ship, npcs, ticks);
    calcNPCShips(&starSystem, &player, npcs, ticks);
    calcEffects(ticks);
    if(hasDockingDistance(&player.ship.position, &starSystem.station.dockingPosition))
    {
        state = STATION;
        player.ship.position = (vec3) {2, 0, 0};
        player.ship.speed *= 0.5f;
    }

    if(shipIsDestroyed(&player.ship))
    {
        vec3 effectPos = addv3(player.ship.position, scalev3(2.5f, multQuatVec3(player.ship.rotation, (vec3) {0, 0, -1})));
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
        moveContractUICursor(-1, stationContracts, numStationContracts);
    }
    else if(keyUp(B_DOWN))
    {
        moveContractUICursor(1, stationContracts, numStationContracts);
    }
    else if(keyUp(B_A))
    {
        if(currentContract.type == CONTRACT_TYPE_NULL)
        {
            uint8_t contractCursor = getContractUICursor();
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
            if(checkContract(&currentContract, &player.hold, currentSystem, npcs))
            {
                uint8_t buffer[64];
                sprintf(buffer, "Contract done.\n%d credits\nhave been\ntransferred.", currentContract.pay);
                createPopup(POPUP_CHECKMARK, buffer);
                completedContracts[currentSystem[0]][currentSystem[1]]++;
                currentContract.type = CONTRACT_TYPE_NULL;
                setContractUICursor(0);
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

            calcShip(&player.ship, ticks);
            if(checkStationCollision(&player.ship))
            {
                //TODO: smooth bounce-back?
                player.ship.shields *= 0.5f;
                player.ship.speed = 0;
            }
            setCameraPos(player.ship.position);
            setCameraRot(player.ship.rotation);

            if(hasLeavingDistance(player.ship.position))
            {
                state = SPACE;
                player.ship.position = starSystem.station.exitPosition;
            }
            else if(hasLandingDistance(player.ship.position))
            {
                state = TRADING;
                player.ship.position.y = 0;
                player.ship.speed = 0;
            }
            break;
        }
        case HYPERSPACE:
        {
            if(currentSystem[0] != getStarmapCursor()[0] || currentSystem[1] != getStarmapCursor()[1])
            {
                player.ship.speed += (500.0f * ticks) / 1000.0f;
                if(player.ship.speed > 500)
                {
                    switchSystem(currentSystem, getStarmapCursor(), &starSystem, npcs);
                    player.ship.position = getRandomFreePos(&starSystem, 20);
                    setInitialSpawnPos(player.ship.position);
                    //Generate contracts for this system
                    generateContractsForSystem(stationContracts, &numStationContracts, &starSystem.info, currentSystem, completedContracts);
                    //Generate new station cargo hold for this system
                    createStationHold(&stationHold);
                    //Set up the current contract (if necessary)
                    contractStarSystemSetup(&currentContract, npcs, currentSystem, &starSystem);
                    //50% chance of decreasing wanted level
                    if(player.wantedLevel > 0 && randr(100) < 50)
                    {
                        player.wantedLevel--;
                    }
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
            calcShip(&player.ship, ticks);
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
                moveEquipUICursor(-1);
            }
            else if(keyUp(B_DOWN))
            {
                moveEquipUICursor(1);
            }
            else if(keyUp(B_A))
            {
                buyEquipment(&player, getEquipUICursor());
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
            
            calcEquipUI(ticks);

            break;
        }
        case CONTRACTS:
        {
            calcContracts();
            calcContractUI(ticks, &currentContract, stationContracts);
            break;
        }
        case MAP:
        {
            int8_t dirX = 0;
            int8_t dirY = 0;
            if(keyUp(B_UP))
            {
                dirY = -1;
            }
            else if(keyUp(B_DOWN))
            {
                dirY = 1;
            }
            if(keyUp(B_LEFT))
            {
                dirX = -1;
            }
            else if(keyUp(B_RIGHT))
            {
                dirX = 1;
            }
            moveStarmapCursor(dirX, dirY);

            if(keyUp(B_START))
            {
                state = SPACE;
            }
            else if(keyUp(B_A))
            {
                float distance = getDistanceToSystem(currentSystem, getStarmapCursor());
                if(player.fuel >= distance)
                {
                    state = HYPERSPACE;
                    player.fuel -= distance;
                    player.ship.turnSpeedX = 0;
                    player.ship.turnSpeedY = 0;
                }
            }

            calcStarmap(ticks);

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
                toggleTitleScreenCursor();
            }
            else if(keyUp(B_A) || keyUp(B_START))
            {
                if(getTitleScreenCursor() == 0)
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

            calcTitleScreen(ticks);

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
        case GAME_OVER:
        {
            drawStarSystem(&starSystem);
            if(starSystem.hasAsteroidField)
            {
                drawAsteroids();
            }
            for(uint8_t i = 0; i < MAX_NPCS; i++)
            {
                if(npcs[i].ship.type != SHIP_TYPE_NULL)
                {
                    drawShip(&npcs[i].ship);
                }
            }
            drawSatellites();
            drawEffects();
            break;
        }
        case STATION:
        {
            drawSpaceStation();
            break;
        }
        case MAP:
        {
            uint8_t contractSystem[2];
            if(currentContract.type != CONTRACT_TYPE_NULL)
            {
                contractSystem[0] = currentContract.targetSystem[0];
                contractSystem[1] = currentContract.targetSystem[1];
            }
            drawStarmap3d(currentSystem, player.fuel, contractSystem, currentContract.type != CONTRACT_TYPE_NULL);
            break;
        }
        case EQUIP:
        {
            drawEquipUI3d(&player);
            break;
        }
        case TITLE:
        {
            drawTitleScreen3d();
            break;
        }
        case CONTRACTS:
        {
            drawContractUI3d(&currentContract, stationContracts);
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
            drawUI(state == STATION, &player, npcs, starSystem.station.position, player.hasAutodock && isAutodockPossible(&player.ship, &starSystem));
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
            drawEquipUI(&player);
            break;
        }
        case CONTRACTS:
        {
            drawContractUI(&currentContract, stationContracts, numStationContracts);
            drawPopupIfActive();
            break;
        }
        case MAP:
        {
            drawSystemInfoBox();
            break;
        }
        case TITLE:
        {
            drawTitleScreen();
            drawPopupIfActive();
            break;
        }
        case GAME_OVER:
        {
            //Keep drawing game UI
            drawUI(state == STATION, &player, npcs, starSystem.station.position, player.hasAutodock && isAutodockPossible(&player.ship, &starSystem));
            drawGameOverScreen();
            break;
        }
    }

    flipFrame();
}

void initGame()
{
    GLuint uiTexture = initUI();
    initPopup(uiTexture);
    initStarmap(uiTexture);
    initEquipUI(uiTexture);
    initTitleScreen();
    initContractUI(uiTexture);

    initEffects();

    initStarSystem();
    initSpaceStation();
    initAsteroids();
    initSatellites();
    initShip();
}

void quitGame()
{
    quitUI();
    quitPopup();
    quitStarmap();
    quitEquipUI();
    quitTitleScreen();
    quitContractUI();

    quitEffects();

    quitStarSystem();
    quitSpaceStation();
    quitAsteroids();
    quitSatellites();
    quitShip();
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

    //Initialize game systems
    initGame();

    initUniverse(&starSystem);
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

    quitGame();

    quitAudio();
	quitVideo();

	return 0;
}