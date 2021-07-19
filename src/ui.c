#include "ui.h"
#include "engine/model.h"
#include "engine/image.h"
#include "cargo.h"

#include "engine/includes/3dMath.h"

SDL_Surface* mainUI;
SDL_Surface* blockBlue;
SDL_Surface* indicator;

GLuint quad;
GLuint mainTexture;

void initUI()
{
    initPNG();
    mainUI = loadPNG("res/UI/main.png");
    blockBlue = loadPNG("res/UI/blockBlue.png");
    indicator = loadPNG("res/UI/indicator.png");

    mainTexture = loadRGBTexture("res/UI/main-2.png");
}

/**
 * Pixel To Coordinate
 * Converts a pixel position (0-255) to a texture coordinate (0-1)
 **/
#define PTC(X) (X / 256.0f)

void drawTexQuad(float posX, float posY, float sizeX, float sizeY, float z,
                    float texX1, float texY1, float texX2, float texY2)
{
    glTexCoord2f(texX1, texY2);
    glVertex3f(posX, posY, z);
    glTexCoord2f(texX2, texY2);
    glVertex3f(posX + sizeX, posY, z);
    glTexCoord2f(texX2, texY1);
    glVertex3f(posX + sizeX, posY + sizeY, z);
    glTexCoord2f(texX1, texY1);
    glVertex3f(posX, posY + sizeY, z);
}

void drawUI(Ship* playerShip)
{
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, mainTexture);
    glBegin(GL_QUADS);
    //Draw main UI backgroundre
    drawTexQuad(0, 0, 240, 240, -2, 0, 0, PTC(240), PTC(240));

    uint8_t speedTemp = (playerShip->speed / playerShip->type->maxSpeed) * 16;
    if(speedTemp > 0)
    {
        drawTexQuad(171, 26, speedTemp * 4, 4, -1, 0, PTC(248), PTC(4) * speedTemp, PTC(251));
    }

    int8_t turnXTemp = (playerShip->turnSpeedX / playerShip->type->maxTurnSpeed) * 30 + 30;
    drawTexQuad(170 + turnXTemp, 56, 4, 4, -1, PTC(252), 0, 1, PTC(3));

    int8_t turnYTemp = (playerShip->turnSpeedY / playerShip->type->maxTurnSpeed) * 30 + 30;
    drawTexQuad(170 + turnYTemp, 41, 4, 4, -1, PTC(252), 0, 1, PTC(3));
    glEnd();
}

//TODO
void drawTradingUI(Ship* playerShip, uint8_t techLevel)
{
    glDrawText("ITEM UNIT PRICE STATION SHIP", 8, 8, 0xFFFFFF);
    uint8_t i;
    for(i = 0; i < NUM_CARGO_TYPES; i++)
    {
        glDrawText("TEST   KG    10     256    0", 8, 16, 0xFFFFFF);
    }
}