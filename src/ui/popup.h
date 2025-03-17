#ifndef POPUP_H
#define POPUP_H

#include <stdbool.h>
#include <stdint.h>
#include "GL/gl.h"

/**
 * Functions for a popup window
 * Can hold 15x6 characters, plus an icon
 **/

typedef enum {
    POPUP_CHECKMARK = 0,
    POPUP_ATTENTION
} PopupIcon;

void initPopup(GLuint uiTex);
void quitPopup();

void drawPopupIfActive();
void createPopup(PopupIcon icon, char* text);
void closePopup();
bool isPopupOpen();

#endif