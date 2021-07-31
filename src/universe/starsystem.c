#include "starsystem.h"
#include "../engine/model.h"
#include "../engine/image.h"

GLuint planetMesh;
GLuint planetRingMesh;
GLuint planetRingTexture;
GLuint sunTexture;

GLuint stationMesh;
GLuint stationTexture;

void initStarSystem()
{
    planetMesh = loadModelList("res/obj/Planet.obj");
    planetRingMesh = loadModelList("res/obj/Planet_Ring.obj");
    planetRingTexture = loadRGBTexture("res/tex/Planet_Ring.png");
    sunTexture = loadRGBTexture("res/tex/Sun.png");

    stationMesh = loadModelList("res/obj/SpaceStation.obj");
    stationTexture = loadRGBTexture("res/tex/SpaceStation.png");
}

void deleteStarSystem(StarSystem* starSystem)
{
    uint8_t i;
    for(i = 0; i < starSystem->numPlanets; i++)
    {
        deleteRGBTexture(starSystem->planets[i].texture);
    }
}

void drawStarSystem(StarSystem* starSystem)
{
    uint8_t i;

    glBindTexture(GL_TEXTURE_2D, sunTexture);
    for(i = 0; i < starSystem->numStars; i++)
    {
        glPushMatrix();
        glTranslatef(starSystem->stars[i].position.x, starSystem->stars[i].position.y, starSystem->stars[i].position.z);
        if(starSystem->stars[i].size != 1.0f)
        {
            float size = starSystem->stars[i].size;
            glScalef(size, size, size);
        }
        glCallList(planetMesh);
        glPopMatrix();
    }

    for(i = 0; i < starSystem->numPlanets; i++)
    {
        glPushMatrix();
        glTranslatef(starSystem->planets[i].position.x, starSystem->planets[i].position.y, starSystem->planets[i].position.z);
        if(starSystem->planets[i].size != 1.0f)
        {
            float size = starSystem->planets[i].size;
            glScalef(size, size, size);
        }
        glBindTexture(GL_TEXTURE_2D, starSystem->planets[i].texture);
        glCallList(planetMesh);
        if(starSystem->planets[i].hasRing)
        {
            glDisable(GL_CULL_FACE);
            glBindTexture(GL_TEXTURE_2D, planetRingTexture);
            glCallList(planetRingMesh);
            glEnable(GL_CULL_FACE);
        }
        glPopMatrix();
    }

    glBindTexture(GL_TEXTURE_2D, stationTexture);
    glPushMatrix();
    glTranslatef(starSystem->station.position.x, starSystem->station.position.y, starSystem->station.position.z);
    glScalef(5, 5, 5);
    glCallList(stationMesh);
    glPopMatrix();
}

uint8_t hasDockingDistance(vec3* pos, vec3* dockingPos)
{
    if(distance3d(pos, dockingPos) < 2)
    {
        return 1;
    }
    return 0;
}