#include "ui.h"
#include "engine/image.h"

SDL_Surface* mainUI;
SDL_Surface* blockBlue;

void initUI()
{
    initPNG();
    mainUI = loadPNG("res/UI/main.png");
    blockBlue = loadPNG("res/UI/blockBlue.png");
}

void drawUI(SDL_Surface* screen, Ship* playerShip)
{
    blitSurface(screen, mainUI, 0, 0);

    uint8_t speedTemp = (playerShip->speed / playerShip->type->maxSpeed) * 15;
    uint8_t i;
    for(i = 0; i < speedTemp; i++)
    {
        blitSurface(screen, blockBlue, 171 + i * 4, 210);
    }
}