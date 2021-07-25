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

uint8_t hasLeavingDistance(vec3 pos)
{
    if(pos.x > 2.1)
    {
        return 1;
    }
    return 0;
}

uint8_t hasLandingDistance(vec3 pos)
{
    if(pos.y < -0.5f)
    {
        return 1;
    }
    return 0;
}