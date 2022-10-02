#ifndef _POPUP_H
#define _POPUP_H

#include <stdbool.h>
#include <stdint.h>
#include "GL/gl.h"

/**
 * Functions for a popup window
 * Can hold 15x6 characters, plus an icon
 **/

#define POPUP_CHECKMARK 0
#define POPUP_ATTENTION 1

void initPopup(GLuint uiTex);
void quitPopup();

void drawPopupIfActive();
void createPopup(uint8_t icon, char* text);
void closePopup();
bool isPopupOpen();

#endif