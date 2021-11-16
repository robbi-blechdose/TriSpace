#include "ui.h"
#include "engine/model.h"
#include "engine/image.h"
#include "engine/includes/3dMath.h"
#include "cargo.h"
#include "universe/universe.h"
#include "universe/generator.h"

GLuint mainTexture;
GLuint firingTexture;
GLuint stationUITexture;
GLuint mapTexture;

uint8_t cursorX;
uint8_t cursorY;

//Save/Load
uint8_t saveLoadCursor;
//Trading
uint8_t tradeCursor;
//Equip
uint8_t equipCursor;
//Contracts
uint8_t contractCursor;
//Map
uint8_t mapScrollX;
uint8_t mapScrollY;
uint8_t mapCursorX;
uint8_t mapCursorY;
//Title
uint8_t titleCursor;

void initUI()
{
    initPNG();
    mainTexture = loadRGBTexture("res/UI/main.png");
    firingTexture = loadRGBTexture("res/UI/firing.png");
    stationUITexture = loadRGBTexture("res/UI/StationUI.png");
    mapTexture = loadRGBTexture("res/UI/map.png");
    cursorX = 0;
    cursorY = 0;

    saveLoadCursor = 0;
    tradeCursor = 0;
    equipCursor = 0;
    contractCursor = 0;
    mapCursorX = 0;
    mapCursorY = 0;
    titleCursor = 0;
}

void drawTexQuad(float posX, float posY, float sizeX, float sizeY, float z,
                    float texX1, float texY1, float texX2, float texY2)
{
    glTexCoord2f(texX1, texY2);
    glVertex3f(posX, posY, z);
    glTexCoord2f(texX2, texY2);
    glVertex3f(posX + sizeX, posY, z);
    glTexCoord2f(texX2, texY1);
    glVertex3f(posX + sizeX, posY + sizeY - 1, z);
    glTexCoord2f(texX1, texY1);
    glVertex3f(posX, posY + sizeY - 1, z);
}

void moveWithRollover(uint8_t* i, uint8_t max, int8_t dir)
{
    if(dir > 0)
    {
        (*i)++;
        (*i) %= (max + 1);
    }
    else
    {
        if(*i > 0)
        {
            (*i)--;
        }
        else
        {
            (*i) = max;
        }
    }
}

//UI Base Height
#define UIBH 10
//UI Top Height
#define UITH 11

void drawRadarDot(vec3 playerPos, vec3 playerRot, vec3 target, uint8_t color)
{
    vec3 diff = subv3(playerPos, target);
    vec3 axis = {.d = {0, 1, 0}};
    vec3 rot = rotatev3(diff, axis, playerRot.y);
    axis.d[0] = 1;
    axis.d[1] = 0;
    rot = rotatev3(rot, axis, playerRot.x);
    rot = normalizev3(rot);

    if(rot.d[2] > 0)
    {
        rot.d[0] = -rot.d[0] * 30;
        rot.d[1] = -rot.d[1] * 30;
    }
    else
    {
        float angle = atan2f(-rot.d[1], -rot.d[0]);
        rot.x = 32 * cosf(angle);
        rot.y = 32 * sinf(angle);
    }

    float texY1 = PTC(4 + color * 4);
    float texY2 = PTC(7 + color * 4);
    drawTexQuad(119.5f + rot.x - 2, 36.5f + rot.y - 2, 4, 4, UITH, PTC(252), texY1, 1, texY2);
}

void drawUI(State state, Ship* playerShip, Ship npcShips[], vec3 stationPos, uint8_t displayContract, vec3 contractPos)
{
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, mainTexture);
    glBegin(GL_QUADS);
    //Draw main UI background
    drawTexQuad(0, 0, 240, 240, UIBH, 0, 0, PTC(240), PTC(240));
    uint8_t speedTemp = (playerShip->speed / shipTypes[playerShip->type].maxSpeed) * 16;
    if(speedTemp > 0)
    {
        drawTexQuad(171, 26, speedTemp * 4, 4, UITH, 0, PTC(249), PTC(4) * speedTemp, PTC(251));
    }

    int8_t turnXTemp = (playerShip->turnSpeedX / shipTypes[playerShip->type].maxTurnSpeed) * 30 + 30;
    drawTexQuad(170 + turnXTemp, 56, 4, 4, UITH, PTC(252), 0, 1, PTC(3));

    int8_t turnYTemp = (playerShip->turnSpeedY / shipTypes[playerShip->type].maxTurnSpeed) * 30 + 30;
    drawTexQuad(170 + turnYTemp, 41, 4, 4, UITH, PTC(252), 0, 1, PTC(3));

    uint8_t shieldsTemp = (playerShip->shields / shipTypes[playerShip->type].maxShields) * 16;
    if(shieldsTemp > 0)
    {
        drawTexQuad(7, 57, shieldsTemp * 4, 4, UITH, 0, PTC(253), PTC(4) * shieldsTemp, 1);
    }

    uint8_t energyTemp = (playerShip->energy / shipTypes[playerShip->type].maxEnergy) * 16;
    if(energyTemp > 0)
    {
        drawTexQuad(7, 42, energyTemp * 4, 4, UITH, 0, PTC(253), PTC(4) * energyTemp, 1);
    }

    uint8_t fuelTemp = ((float) playerShip->fuel / MAX_FUEL) * 16;
    if(fuelTemp > 0)
    {
        drawTexQuad(7, 27, fuelTemp * 4, 4, UITH, 0, PTC(246), PTC(4) * fuelTemp, PTC(248));
    }

    //Radar
    if(state != STATION)
    {
        drawRadarDot(playerShip->position, playerShip->rotation, stationPos, 1);

        if(displayContract)
        {
            drawRadarDot(playerShip->position, playerShip->rotation, contractPos, 2);
        }

        for(uint8_t i = 0; i < MAX_NPC_SHIPS; i++)
        {
            if(npcShips[i].type != TYPE_NULL)
            {
                if(distance3d(&playerShip->position, &npcShips[i].position) < RADAR_RANGE)
                {
                    drawRadarDot(playerShip->position, playerShip->rotation, npcShips[i].position, 0);
                }
            }
        }
    }

    //Draw effect if firing
    if(playerShip->weapon.timer)
    {
        glBindTexture(GL_TEXTURE_2D, firingTexture);
        drawTexQuad(2, 72, 237, 167, UIBH, 0, 0, PTC(235), PTC(165));
    }

    glEnd();
}

void drawSaveLoadUI()
{
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, stationUITexture);
    glBegin(GL_QUADS);
    drawTexQuad(0, 0, 240, 240, UIBH, 0, 0, PTC(240), PTC(240));
    //Save/Load icons
    drawTexQuad(CENTER(13), 200, 8, 8, UITH, PTC(241), 0, PTC(249), PTC(8));
    drawTexQuad(CENTER(13), 184, 8, 8, UITH, PTC(249), 0, PTC(256), PTC(8));
    glEnd();
    glDrawText("Save & Load", CENTER(11), 2, 0xFFFFFF);

    char buffer[29];

    if(saveLoadCursor == 0)
    {
        glDrawText("Save game", CENTER(9), 32, 0x00FFFF);
        glDrawText("Load game", CENTER(9), 48, 0xFFFFFF);
    }
    else
    {
        glDrawText("Save game", CENTER(9), 32, 0xFFFFFF);
        glDrawText("Load game", CENTER(9), 48, 0x00FFFF);
    }

    glDrawText("Trading", 240 - 7 * 8 - 12, 240 - 10, 0xFFFFFF);
}

void toggleSaveLoadCursor()
{
    if(saveLoadCursor == 0)
    {
        saveLoadCursor = 1;
    }
    else
    {
        saveLoadCursor = 0;
    }
}

uint8_t getSaveLoadCursor()
{
    return saveLoadCursor;
}

void drawTradingUI(CargoHold* playerHold, CargoHold* stationHold, SystemInfo* info)
{
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, stationUITexture);
    glBegin(GL_QUADS);
    drawTexQuad(0, 0, 240, 240, UIBH, 0, 0, PTC(240), PTC(240));
    glEnd();
    glDrawText("Trading", CENTER(7), 2, 0xFFFFFF);

    char buffer[29];
    char name[15];
    char type[4];

    glDrawText("ITEM        UNIT PRICE    QTY", 4, 16, 0xFFFFFF);
    for(uint8_t i = 0; i < NUM_CARGO_TYPES; i++)
    {
        printNameForCargo(name, i);
        printUnitForCargo(type, i);
        sprintf(buffer, "%-13s%3s %5d  %2d|%2d", name, type, getPriceForCargo(i, info),
                                                    stationHold->cargo[i], playerHold->cargo[i]);
        if(i == tradeCursor)
        {
            glDrawText(buffer, 4, 24 + i * 8, 0x00FFFF);
        }
        else
        {
            glDrawText(buffer, 4, 24 + i * 8, 0xFFFFFF);
        }
    }

    sprintf(buffer, "%d credits", playerHold->money);
    glDrawText(buffer, CENTER(strlen(buffer)), 218, 0xFFFFFF);

    glDrawText("Save & Load", 12, 240 - 10, 0xFFFFFF);
    glDrawText("Equip ship", 240 - 10 * 8 - 12, 240 - 10, 0xFFFFFF);
}

void moveTradeCursor(int8_t dir)
{
    moveWithRollover(&tradeCursor, NUM_CARGO_TYPES - 1, dir);
}

uint8_t getTradeCursor()
{
    return tradeCursor;
}

uint16_t equipmentPrices[3] = {
    2,
    1500,
    2000
};

void drawEquipUI(Ship* playerShip)
{
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, stationUITexture);
    glBegin(GL_QUADS);
    drawTexQuad(0, 0, 240, 240, UIBH, 0, 0, PTC(240), PTC(240));
    glEnd();
    glDrawText("Equip ship", CENTER(10), 2, 0xFFFFFF);

    char buffer[29];

    glDrawText("ITEM                PRICE QTY", 4, 16, 0xFFFFFF);
    for(uint8_t i = 0; i < NUM_EQUIPMENT; i++)
    {
        switch(i)
        {
            case EQUIP_FUEL:
            {
                sprintf(buffer, "Fuel (0.5)           %4d %.1f", equipmentPrices[i], playerShip->fuel / 10.0f);
                break;
            }
            case EQUIP_HOLD30:
            {
                char* own = " / ";
                char* own2 = "OWN";
                if(playerShip->hold.size >= 30)
                {
                    own = own2;
                }
                sprintf(buffer, "30 unit Cargo Hold   %4d %s", equipmentPrices[i], own);
                break;
            }
            case EQUIP_MK2LASER:
            {
                char* own = " / ";
                char* own2 = "OWN";
                if(0) //TODO: Check weapon type
                {
                    own = own2;
                }
                sprintf(buffer, "MkII laser           %4d %s", equipmentPrices[i], own);
                break;
            }
        }
        if(i == equipCursor)
        {
            glDrawText(buffer, 4, 24 + i * 8, 0x00FFFF);
        }
        else
        {
            glDrawText(buffer, 4, 24 + i * 8, 0xFFFFFF);
        }
    }
    
    sprintf(buffer, "%d credits", playerShip->hold.money);
    glDrawText(buffer, CENTER(strlen(buffer)), 218, 0xFFFFFF);

    glDrawText("Trading", 12, 240 - 10, 0xFFFFFF);
    glDrawText("Contracts", 240 - 9 * 8 - 12, 240 - 10, 0xFFFFFF);
}

uint8_t getEquipCursor()
{
    return equipCursor;
}

void moveEquipCursor(int8_t dir)
{
    moveWithRollover(&equipCursor, NUM_EQUIPMENT - 1, dir);
}

void drawContract(Contract* contract, uint8_t cursor, uint32_t systemSeeds[], uint8_t numContracts)
{
    glBegin(GL_QUADS);
    drawTexQuad(4, 194, 32, 32, UITH, PTC(240), PTC(16 + contract->type * 16),
                                        1, PTC(31 + contract->type * 16));
    glEnd();
    char buffer[29];
    if(cursor)
    {
        sprintf(buffer, "%d/%d", cursor, numContracts);
        glDrawText(buffer, 8, 48, 0xFFFFFF);
    }
    else
    {
        glDrawText("ACT", 8, 48, 0x00FF00);
    }

    glDrawText(contractTypes[contract->type], 40, 16, 0xFFFFFF);
    sprintf(buffer, "Employer: %s %s", contractFirstnames[contract->employerFirstname],
                                        contractLastnames[contract->employerLastname]);
    glDrawText(buffer, 40, 32, 0xFFFFFF);
    sprintf(buffer, "Pay: %d credits", contract->pay);
    glDrawText(buffer, 40, 48, 0xFFFFFF);
    glDrawText("Destination system:", 40, 64, 0xFFFFFF);

    SystemBaseData sbd;
    generateSystemBaseData(&sbd, systemSeeds[contract->targetSystem]);
    glDrawText(sbd.info.name, 40, 72, 0xFFFFFF);

    glDrawText("Objective:", 40, 88, 0xFFFFFF);
    printObjective(buffer, contract);
    glDrawText(buffer, 40, 96, 0xFFFFFF);
}

void drawContractUI(Contract* activeContract, Contract* contracts, uint32_t systemSeeds[], uint8_t numContracts)
{
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, stationUITexture);
    glBegin(GL_QUADS);
    drawTexQuad(0, 0, 240, 240, UIBH, 0, 0, PTC(240), PTC(240));
    glEnd();
    glDrawText("Contracts", CENTER(9), 2, 0xFFFFFF);

    if(activeContract->type != CONTRACT_TYPE_NULL)
    {
        drawContract(activeContract, 0, systemSeeds, numContracts);
    }
    else
    {
        drawContract(&contracts[contractCursor], contractCursor + 1, systemSeeds, numContracts);
    }

    glDrawText("Equip ship", 12, 240 - 10, 0xFFFFFF);
}

void resetContractCursor()
{
    contractCursor = 0;
}

void moveContractCursor(int8_t dir, uint8_t numContracts)
{
    moveWithRollover(&contractCursor, numContracts - 1, dir);
}

uint8_t getContractCursor()
{
    return contractCursor;
}

void drawMap(uint32_t systemSeeds[], float fuel)
{
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, mapTexture);
    glBegin(GL_QUADS);
    //Draw background
    drawTexQuad(0, 0, 240, 240, UIBH, 0, 0, PTC(240), PTC(240));

    vec2 systemPos;
    for(uint8_t i = mapScrollX; i < mapScrollX + 4; i++)
    {
        for(uint8_t j = mapScrollY; j < mapScrollY + 3; j++)
        {
            uint8_t numStars = getNumStarsForSystem(systemSeeds[i + j * 16]) - 1;
            generateSystemPos(&systemPos, systemSeeds[i + j * 16], i, j);
            drawTexQuad(8 + systemPos.x - (mapScrollX * 64),
                        80 + systemPos.y - (mapScrollY * 64),
                        16, 16, UITH, PTC(241), PTC(numStars * 16), 1, PTC(15 + numStars * 16));
        }
    }
    generateSystemPos(&systemPos, systemSeeds[mapCursorX + mapCursorY * 16], mapCursorX, mapCursorY);
    if(getDistanceToSystem(mapCursorX + mapCursorY * 16) <= fuel)
    {
        //Green, we can go there
        drawTexQuad(8 + systemPos.x - (mapScrollX * 64),
                    80 + systemPos.y - (mapScrollY * 64), 16, 16, UITH, PTC(241), PTC(48), 1, PTC(63));
    }
    else
    {
        //Red, too far away
        drawTexQuad(8 + systemPos.x - (mapScrollX * 64),
                    80 + systemPos.y - (mapScrollY * 64), 16, 16, UITH, PTC(241), PTC(64), 1, PTC(80));
    }

    //System info box
    SystemBaseData sbd;
    generateSystemBaseData(&sbd, systemSeeds[getMapCursor()]);
    char buffer[29];
    glDrawText("System information", 48, 195, 0xFFFFFF);
    glDrawText(sbd.info.name, CENTER(strlen(sbd.info.name)), 204, 0xFFFFFF);
    sprintf(buffer, "Tech level: %d", sbd.info.techLevel);
    glDrawText(buffer, 8, 213, 0xFFFFFF);
    sprintf(buffer, "Government level: %d", sbd.info.government);
    glDrawText(buffer, 8, 222, 0xFFFFFF);
    glDrawText("Planets:", 8, 231, 0xFFFFFF);
    for(uint8_t i = 0; i < sbd.numPlanets; i++)
    {
        drawTexQuad(80 + i * 12, 1, 7, 8, UITH,
                    PTC(241), PTC(80 + sbd.paletteIndices[i] * 8), PTC(248), PTC(87 + sbd.paletteIndices[i] * 8));
    }
    glEnd();
}

void moveMapCursor(int8_t x, int8_t y)
{
    //TODO: Remove rollover, cap in interval 0 - 15
    if(x != 0)
    {
        moveWithRollover(&mapCursorX, 16, x);
    }
    if(y != 0)
    {
        moveWithRollover(&mapCursorY, 16, y);
    }

    if(mapCursorX >= 2)
    {
        mapScrollX = mapCursorX - 2;
    }
    if(mapCursorY >= 2)
    {
        mapScrollY = mapCursorY - 2;
    }
}

uint16_t getMapCursor()
{
    uint16_t ret = mapCursorY * 16 + mapCursorX;
    return ret;
}

//TODO: Title screen texture
void drawTitleScreen()
{
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, stationUITexture);
    glBegin(GL_QUADS);
    drawTexQuad(0, 0, 240, 240, UIBH, 0, 0, PTC(240), PTC(240));
    glEnd();
    glDrawText("TriSpace", CENTER(8), 2, 0xFFFFFF);

    if(titleCursor == 0)
    {
        glDrawText("New game", CENTER(8), 32, 0x00FFFF);
        glDrawText("Load game", CENTER(9), 48, 0xFFFFFF);
    }
    else
    {
        glDrawText("New game", CENTER(8), 32, 0xFFFFFF);
        glDrawText("Load game", CENTER(9), 48, 0x00FFFF);
    }
}

void toggleTitleCursor()
{
    if(titleCursor != 0)
    {
        titleCursor = 0;
    }
    else
    {
        titleCursor = 1;
    }
}

uint8_t getTitleCursor()
{
    return titleCursor;
}