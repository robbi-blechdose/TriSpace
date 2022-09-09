#include "starmap.h"

#include "../engine/model.h"
#include "../engine/image.h"

#include "../universe/generator.h"
#include "../universe/universe.h"

/**
const char* governmentLevels[5] = {
    "Anarchy",
    "Feudal",
    "Dictatorship",
    "Corporate state",
    "Democracy"
};**/

GLuint starMesh;
GLuint starTexture;

uint8_t cursor[2];

void initStarmap()
{
    starMesh = loadModelList("res/obj/Planet.obj");
    starTexture = loadRGBTexture("res/tex/Sun.png");
}

void calcStarmap(uint8_t currentSystem[2], float fuel)
{

}

void drawSystemInfoBox()
{
    /**
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, mapTexture);
    glBegin(GL_QUADS);
    //Background
    drawTexQuad(0, 0, 240, 240, UIBH, 0, 0, PTC(240), PTC(240));
    //Content
    SystemBaseData sbd;
    generateSystemBaseData(&sbd, getSeedForSystem(cursor[0], cursor[1]));

    char buffer[29];
    glDrawText("System information", 48, 195, 0xFFFFFF);
    glDrawText(sbd.info.name, CENTER(strlen(sbd.info.name)), 204, 0xFFFFFF);
    sprintf(buffer, "Tech level: %d", sbd.info.techLevel);
    glDrawText(buffer, 8, 213, 0xFFFFFF);
    sprintf(buffer, "Government: %s", governmentLevels[sbd.info.government]);
    glDrawText(buffer, 8, 222, 0xFFFFFF);
    glDrawText("Planets:", 8, 231, 0xFFFFFF);
    for(uint8_t i = 0; i < sbd.numPlanets; i++)
    {
        drawTexQuad(80 + i * 12, 1, 7, 8, UITH,
                    PTC(241), PTC(80 + sbd.paletteIndices[i] * 8), PTC(248), PTC(87 + sbd.paletteIndices[i] * 8));
    }
    glEnd();**/
}

void drawStarmap()
{
    glBindTexture(GL_TEXTURE_2D, starTexture);

    vec2 systemPos;
    for(uint8_t i = 0; i < UNIVERSE_SIZE; i++)
    {
        for(uint8_t j = 0; j < UNIVERSE_SIZE; j++)
        {
            uint8_t numStars = getNumStarsForSystem(getSeedForSystem(i, j)) - 1;
            generateSystemPos(&systemPos, getSeedForSystem(i, j), i, j);

            //TODO: multiple star drawing
            glPushMatrix();
            glTranslatef(systemPos.x * 50, 0, systemPos.y * 50);
            glCallList(starMesh);
            glPopMatrix();
        }
    }

    drawSystemInfoBox();
}