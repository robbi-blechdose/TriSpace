#include "spacestation.h"

#include "../engine/model.h"
#include "../engine/image.h"

GLuint stationInteriorMesh;
GLuint stationInteriorTexture;

void initSpaceStation()
{
    stationInteriorMesh = loadModelList("res/obj/SpaceStation_Interior.obj");
    stationInteriorTexture = loadRGBTexture("res/tex/SpaceStation_Interior.png");
}

void loadSpaceStation()
{

}

void drawSpaceStation()
{
    glBindTexture(GL_TEXTURE_2D, stationInteriorTexture);
    glCallList(stationInteriorMesh);
}

uint8_t hasLeavingDistance(Ship* ship)
{
    if(ship->position.x > 2.1)
    {
        return 1;
    }
    return 0;
}