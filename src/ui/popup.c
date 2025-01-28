#include "popup.h"

#include <string.h>
#include "../fk-engine-core/image.h"
#include "uiutils.h"

static GLuint uiTexture;

bool popupActive;
uint8_t popupIcon;
char popupText[15 * 6];

void initPopup(GLuint uiTex)
{
    uiTexture = uiTex;
}

void quitPopup() {}

void drawPopupIfActive()
{
    if(!popupActive)
    {
        return;
    }

    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, uiTexture);
    glBegin(GL_QUADS);
    drawTexQuad(40, 80, 160, 80, UIPH, 0, PTC(144), PTC(159), PTC(223));
    drawTexQuad(48, 136, 16, 16, UIPTH, PTC(16 * popupIcon), PTC(224), PTC(16 + 16 * popupIcon), PTC(239));
    glDrawText(popupText, 72, 88, TEXT_WHITE);
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