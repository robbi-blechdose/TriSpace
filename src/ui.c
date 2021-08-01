#include "ui.h"
#include "engine/model.h"
#include "engine/image.h"
#include "engine/includes/3dMath.h"
#include "cargo.h"
#include "universe/universe.h"
#include "universe/generator.h"

GLuint mainTexture;
GLuint tradeTexture;
GLuint mapTexture;

//Trading
uint8_t tradeCursor;
//Map
float mapScrollX;
float mapScrollY;
uint8_t mapCursorX;
uint8_t mapCursorY;

void initUI()
{
    initPNG();
    mainTexture = loadRGBTexture("res/UI/main.png");
    tradeTexture = loadRGBTexture("res/UI/trading.png");
    mapTexture = loadRGBTexture("res/UI/map.png");
    tradeCursor = 0;
    mapCursorX = 0;
    mapCursorY = 0;
}

/**
 * Pixel To Coordinate
 * Converts a pixel position (0-255) to a texture coordinate (0-1)
 **/
#define PTC(X) ((X) / 256.0f)

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

//UI Base Height
#define UIBH 10
//UI Top Height
#define UITH 11

void drawRadarDot(vec3 playerPos, vec3 playerRot, vec3 target, uint8_t isEnemy)
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

    float texY1 = PTC(4);
    float texY2 = PTC(7);
    if(!isEnemy)
    {
        texY1 = PTC(8);
        texY2 = PTC(11);
    }
    drawTexQuad(119.5f + rot.x - 2, 36.5f + rot.y - 2, 4, 4, UITH, PTC(252), texY1, 1, texY2);
}

void drawUI(State state, Ship* playerShip, Ship npcShips[], vec3 stationPos)
{
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, mainTexture);
    glBegin(GL_QUADS);
    //Draw main UI background
    drawTexQuad(0, 0, 240, 240, UIBH, 0, 0, PTC(240), PTC(240));

    uint8_t speedTemp = (playerShip->speed / playerShip->type->maxSpeed) * 16;
    if(speedTemp > 0)
    {
        drawTexQuad(171, 26, speedTemp * 4, 4, UITH, 0, PTC(249), PTC(4) * speedTemp, PTC(251));
    }

    int8_t turnXTemp = (playerShip->turnSpeedX / playerShip->type->maxTurnSpeed) * 30 + 30;
    drawTexQuad(170 + turnXTemp, 56, 4, 4, UITH, PTC(252), 0, 1, PTC(3));

    int8_t turnYTemp = (playerShip->turnSpeedY / playerShip->type->maxTurnSpeed) * 30 + 30;
    drawTexQuad(170 + turnYTemp, 41, 4, 4, UITH, PTC(252), 0, 1, PTC(3));

    uint8_t shieldsTemp = (playerShip->shields / playerShip->type->maxShields) * 16;
    if(shieldsTemp > 0)
    {
        drawTexQuad(7, 57, shieldsTemp * 4, 4, UITH, 0, PTC(253), PTC(4) * shieldsTemp, 1);
    }

    uint8_t energyTemp = (playerShip->energy / playerShip->type->maxEnergy) * 16;
    if(energyTemp > 0)
    {
        drawTexQuad(7, 42, energyTemp * 4, 4, UITH, 0, PTC(253), PTC(4) * energyTemp, 1);
    }

    //Radar
    if(state != STATION)
    {
        drawRadarDot(playerShip->position, playerShip->rotation, stationPos, 0);

        for(uint8_t i = 0; i < MAX_NPC_SHIPS; i++)
        {
            if(npcShips[i].type != NULL)
            {
                if(distance3d(&playerShip->position, &npcShips[i].position) < 50)
                {
                    drawRadarDot(playerShip->position, playerShip->rotation, npcShips[i].position, 1);
                }
            }
        }
    }
    glEnd();
}

void drawTradingUI(CargoHold* playerHold, CargoHold* stationHold, SystemInfo* info)
{
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, tradeTexture);
    glBegin(GL_QUADS);
    //Draw rect background
    drawTexQuad(0, 0, 240, 240, UIBH, 0, 0, PTC(240), PTC(240));
    glEnd();

    char buffer[29];

    glDrawText("ITEM       UNIT PRICE     QTY", 8, 8, 0xFFFFFF);
    for(uint8_t i = 0; i < NUM_CARGO_TYPES; i++)
    {
        printNameForCargo(buffer, i);
        printUnitForCargo(&buffer[12], i);
        sprintf(&buffer[15], " %5d  %2d|%2d", getPriceForCargo(i, info), stationHold->cargo[i], playerHold->cargo[i]);
        if(i == tradeCursor)
        {
            glDrawText(buffer, 8, 16 + i * 8, 0x000000);
        }
        else
        {
            glDrawText(buffer, 8, 16 + i * 8, 0xFFFFFF);
        }
    }

    sprintf(buffer, "%d credits", playerHold->money);
    glDrawText(buffer, 8, 224, 0xFFFFFF);
}

void moveWithRollover(uint8_t* i, uint8_t max, int8_t dir)
{
    if(dir > 0)
    {
        if(*i < max)
        {
            (*i)++;
        }
        else
        {
            (*i) = 0;
        }
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

void moveTradeCursor(int8_t dir)
{
    moveWithRollover(&tradeCursor, NUM_CARGO_TYPES - 1, dir);
}

uint8_t getTradeCursor()
{
    return tradeCursor;
}

void drawMap(uint32_t systemSeeds[])
{
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, mapTexture);
    glBegin(GL_QUADS);
    //Draw background
    drawTexQuad(0, 0, 240, 240, UIBH, 0, 0, PTC(240), PTC(240));

    for(uint8_t i = 0; i < 16; i++)
    {
        for(uint8_t j = 0; j < 16; j++)
        {
            uint8_t numStars = getNumStarsForSystem(systemSeeds[i + j * 16]) - 1;
            drawTexQuad(8 + i * 48, 56 + j * 48, 16, 16, UITH, PTC(241), PTC(numStars * 16), 1, PTC(15 + numStars * 16));
        }
    }
    drawTexQuad(8 + mapCursorX * 48, 56 + mapCursorY * 48, 16, 16, UITH, PTC(241), PTC(48), 1, PTC(63));

    //System info box
    SystemBaseData sbd;
    generateSystemBaseData(&sbd, systemSeeds[getMapCursor()]);
    char buffer[29];
    glDrawText("System information", 8, 200, 0xFFFFFF);
    sprintf(buffer, "Tech level: %d", sbd.info.techLevel);
    glDrawText(buffer, 8, 208, 0xFFFFFF);
    for(uint8_t i = 0; i < sbd.numPlanets; i++)
    {
        drawTexQuad(8 + i * 12, 8, 7, 8, UITH,
                    PTC(241), PTC(80 + sbd.paletteIndices[i] * 8), PTC(248), PTC(87 + sbd.paletteIndices[i] * 8));
    }
    glEnd();
}

void moveMapCursor(int8_t x, int8_t y)
{
    if(x != 0)
    {
        moveWithRollover(&mapCursorX, 16, x);
    }
    if(y != 0)
    {
        moveWithRollover(&mapCursorY, 16, y);
    }
}

uint16_t getMapCursor()
{
    uint16_t ret = mapCursorY * 16 + mapCursorX;
    return ret;
}