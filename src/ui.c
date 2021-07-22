#include "ui.h"
#include "engine/model.h"
#include "engine/image.h"
#include "engine/includes/3dMath.h"
#include "cargo.h"
#include "universe/universe.h"

GLuint mainTexture;
GLuint rectTexture;

void initUI()
{
    initPNG();
    mainTexture = loadRGBTexture("res/UI/main.png");
    rectTexture = loadRGBTexture("res/UI/rect.png");
}

/**
 * Pixel To Coordinate
 * Converts a pixel position (0-255) to a texture coordinate (0-1)
 **/
#define PTC(X) (X / 256.0f)

//TODO: Fix stuff being 1 pixel too tall (wtf???)

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

void drawUI(State state, Ship* playerShip, Ship npcShips[])
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
        for(uint8_t i = 0; i < MAX_NPC_SHIPS; i++)
        {
            if(npcShips[i].type != NULL)
            {
                if(distance3d(&playerShip->position, &npcShips[i].position) < 50)
                {
                    vec3 player = {.d = {playerShip->position.x, playerShip->position.y, playerShip->position.z}};
                    vec3 enemy = {.d = {npcShips[i].position.x, npcShips[i].position.y, npcShips[i].position.z}};
                    vec3 diff = subv3(player, enemy);
                    vec3 axis = {.d = {0, 1, 0}};
                    vec3 rot = rotatev3(diff, axis, playerShip->rotation.y);
                    axis.d[0] = 1;
                    axis.d[1] = 0;
                    rot = rotatev3(rot, axis, playerShip->rotation.x);
                    rot = normalizev3(rot);

                    if(rot.d[2] > 0)
                    {
                        rot.d[0] = -rot.d[0] * 30;
                        rot.d[1] = -rot.d[1] * 30;
                    }
                    else
                    {
                        float angle = atan2f(-rot.d[1], -rot.d[0]);
                        rot.d[0] = 32 * cosf(angle);
                        rot.d[1] = 32 * sinf(angle);
                    }

                    drawTexQuad(119.5f + rot.d[0] - 2, 36.5f + rot.d[1] - 2, 4, 4, UITH,
                                PTC(252), PTC(4), 1, PTC(7));
                }
            }
        }
    }
    glEnd();
}

//TODO
void drawTradingUI(Ship* playerShip, uint8_t techLevel)
{
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, rectTexture);
    glBegin(GL_QUADS);
    //Draw rect background
    drawTexQuad(0, 0, 240, 240, -2, 0, 0, PTC(240), PTC(240));

    glDrawText("ITEM UNIT PRICE STATION SHIP", 8, 8, 0xFFFFFF);
    uint8_t i;
    for(i = 0; i < NUM_CARGO_TYPES; i++)
    {
        glDrawText("TEST   KG    10     256    0", 8, 16, 0xFFFFFF);
    }
    glEnd();
}