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

#define MENU_SIZE 4
const char* menuStrings[MENU_SIZE] = {
    "Continue",
    "New game",
    "Credits",
    "Quit"
};

void initTitleScreen()
{
    titleShipMesh = loadModelList("res/obj/ships/Ship.obj");
    titleShipTexture = loadRGBTexture("res/tex/ships/Ship.png");
}

void quitTitleScreen()
{
    glDeleteList(titleShipMesh);
    deleteRGBTexture(titleShipTexture);
}

void calcTitleScreen(uint32_t ticks)
{
    titleShipRot += (45 * ticks) / 1000.0f;
    //Clamp values to keep accuracy even when we rotate for a long time
    if(titleShipRot >= 360)
    {
        titleShipRot -= 360;
    }

    setCameraPos((vec3) {0, 0.4f, 5.5f});
    setCameraRot(quatFromAngles((vec3) {0, 0, 0}));
}

void drawTitleScreen3d()
{
    glBindTexture(GL_TEXTURE_2D, titleShipTexture);

    glRotatef(titleShipRot, 0, 1, 0);
    glRotatef(titleShipRot, 0, 0, 1);
    glCallList(titleShipMesh);
}

void drawTitle()
{
    glLoadIdentity();
    //Title and version
    glTextSize(GL_TEXT_SIZE16x16);
    glDrawText("TriSpace", CENTER(8 * 2), 12, 0xFFFFFF);
    glTextSize(GL_TEXT_SIZE8x8);
    glDrawText(GAME_VERSION, CENTER(strlen(GAME_VERSION)), 32, 0xFFFFFF);
}

void drawTitleScreen()
{
    drawTitle();

    //Menu selections
    for(uint8_t i = 0; i < MENU_SIZE; i++)
    {
        glDrawText(menuStrings[i], CENTER(strlen(menuStrings[i])), 146 + i * 16, cursor == i ? 0x00FFFF : TEXT_WHITE);
    }

    glDrawText("2021 - 2022", CENTER(11), 240 - 28, 0xFFFFFF);
    glDrawText("Robbi Blechdose", CENTER(15), 240 - 16, 0xFFFFFF);
}

void scrollTitleScreenCursor(int8_t dir)
{
    cursor += dir;

    if(cursor < 0)
    {
        cursor = MENU_SIZE - 1;
    }
    else if(cursor >= MENU_SIZE)
    {
        cursor = 0;
    }
}


int8_t getTitleScreenCursor()
{
    return cursor;
}

//Number of lines onscreen at once
#define CREDITS_WINDOW 8
static uint8_t creditsIndex;
#define CREDITS_SCROLL_TIME 700
static uint16_t creditsTimer;

#define CREDITS_LINES 19

const char* creditsLines[CREDITS_LINES] = {
    "",
    "",
    "Programming and Graphics:",
    "  Robbi Blechdose",
    "",
    "Explosion effect",
    "  by MattWalkden",
    "",
    "Music:",
    "The Blue Danube",
    "  by Musopen",
    "Menu Loops Rock",
    "  by Yubatake",
    "  from opengameart.org",
    "",
    "SFX:",
    "  by Q009",
    "  by Michel Baradari",
    "  from opengameart.org"
};

bool calcCredits(uint32_t ticks)
{
    creditsTimer += ticks;
    if(creditsTimer >= CREDITS_SCROLL_TIME)
    {
        creditsTimer = 0;
        creditsIndex++;
    }

    if(creditsIndex == CREDITS_LINES)
    {
        creditsIndex = 0;
        return true;
    }
    
    return false;
}


void drawCredits()
{
    drawTitle();

    for(uint8_t i = 0; i < CREDITS_WINDOW; i++)
    {
        if(creditsIndex + i < CREDITS_LINES)
        {
            glDrawText(creditsLines[creditsIndex + i], CENTER(strlen(creditsLines[creditsIndex + i])), 72 + i * 16, TEXT_WHITE);
        }
    }
}