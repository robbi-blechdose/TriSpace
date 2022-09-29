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

void quitSpaceStation()
{
    glDeleteList(stationInteriorMesh);
    deleteRGBTexture(stationInteriorTexture);
}

void drawSpaceStation()
{
    glBindTexture(GL_TEXTURE_2D, stationInteriorTexture);
    glCallList(stationInteriorMesh);
}

bool hasLeavingDistance(vec3 pos)
{
    return pos.x > 2.6f;
}

bool hasLandingDistance(vec3 pos)
{
    return pos.y < -0.85f;
}