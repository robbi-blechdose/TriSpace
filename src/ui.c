#include "ui.h"
#include "engine/image.h"

SDL_Surface* mainUI;
SDL_Surface* blockBlue;
SDL_Surface* indicator;

void initUI()
{
    initPNG();
    mainUI = loadPNG("res/UI/main.png");
    blockBlue = loadPNG("res/UI/blockBlue.png");
    indicator = loadPNG("res/UI/indicator.png");
}

void drawUI(SDL_Surface* screen, Ship* playerShip)
{
    blitSurface(screen, mainUI, 0, 0);

    uint8_t speedTemp = (playerShip->speed / playerShip->type->maxSpeed) * 16;
    uint8_t i;
    for(i = 0; i < speedTemp; i++)
    {
        blitSurface(screen, blockBlue, 170 + i * 4, 210);
    }

    int8_t turnXTemp = (playerShip->turnSpeedX / playerShip->type->maxTurnSpeed) * 30 + 30;
    blitSurface(screen, indicator, 170 + turnXTemp, 180);
    int8_t turnYTemp = (playerShip->turnSpeedY / playerShip->type->maxTurnSpeed) * 30 + 30;
    blitSurface(screen, indicator, 170 + turnYTemp, 195);
}