#include "titlescreen.h"

#include "../engine/model.h"
#include "../engine/image.h"
#include "../engine/camera.h"

#include "uiutils.h"
#include "../version.h"

static int8_t cursor = 0;

GLuint titleShipMesh;
GLuint titleShipTexture;

static float titleShipRot = 0;

void initTitleScreen()
{
    titleShipMesh = loadModelList("res/obj/ships/Ship.obj");
    titleShipTexture = loadRGBTexture("res/tex/ships/Ship.png");
}

void calcTitleScreen(uint32_t ticks)
{
    titleShipRot += (45 * ticks) / 1000.0f;
    //Clamp values to keep accuracy even when we rotate for a long time
    if(titleShipRot >= 360)
    {
        titleShipRot -= 360;
    }

    setCameraPos((vec3) {0, 0.9f, 5});
    setCameraRot(quatFromAngles((vec3) {0, 0, 0}));
}

void drawTitleScreen3d()
{
    glBindTexture(GL_TEXTURE_2D, titleShipTexture);

    glRotatef(titleShipRot, 0, 1, 0);
    glRotatef(titleShipRot, 0, 0, 1);
    glCallList(titleShipMesh);
}

void drawTitleScreen()
{
    glLoadIdentity();
    //Title and version
    glTextSize(GL_TEXT_SIZE16x16);
    glDrawText("TriSpace", CENTER(8 * 2), 20, 0xFFFFFF);
    glTextSize(GL_TEXT_SIZE8x8);
    glDrawText(GAME_VERSION, CENTER(strlen(GAME_VERSION)), 40, 0xFFFFFF);

    if(cursor == 0)
    {
        glDrawText("New game", CENTER(8), 160, 0x00FFFF);
        glDrawText("Continue game", CENTER(13), 176, 0xFFFFFF);
    }
    else
    {
        glDrawText("New game", CENTER(8), 160, 0xFFFFFF);
        glDrawText("Continue game", CENTER(13), 176, 0x00FFFF);
    }

    glDrawText("2021 - 2022", CENTER(11), 240 - 32, 0xFFFFFF);
    glDrawText("Robbi Blechdose", CENTER(15), 240 - 20, 0xFFFFFF);
}

void toggleTitleScreenCursor()
{
    if(cursor)
    {
        cursor = 0;
    }
    else
    {
        cursor = 1;
    }
}


int8_t getTitleScreenCursor()
{
    return cursor;
}