#include "popup.h"

#include "../engine/image.h"

//UI Popup Height
#define UIPH 12
//UI Popup Top Height
#define UIPTH 13

GLuint popupTexture;

bool popupActive;
uint8_t popupIcon;
char popupText[15 * 6];

void initPopup()
{
    popupTexture = loadRGBTexture("res/UI/popup.png");
}

void drawPopupIfActive()
{
    if(!popupActive)
    {
        return;
    }

    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, popupTexture);
    glBegin(GL_QUADS);
    drawTexQuad(40, 80, 160, 80, UIPH, 0, 0, PTC(160), PTC(79));
    drawTexQuad(48, 136, 16, 16, UIPTH, PTC(16 * popupIcon), PTC(80), PTC(16 + 16 * popupIcon), PTC(96));
    glDrawText(popupText, 72, 88, 0xFFFFFF);
    glEnd();
}

void createPopup(uint8_t icon, char* text)
{
    popupActive = true;
    popupIcon = icon;
    strcpy(popupText, text);
}

void closePopup()
{
    popupActive = false;
}

bool isPopupOpen()
{
    return popupActive;
}