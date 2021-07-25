#include "starsystem.h"
#include "../engine/model.h"
#include "../engine/image.h"

StarSystem sys;

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

void deleteStarSystem()
{
    uint8_t i;
    for(i = 0; i < sys.numPlanets; i++)
    {
        deleteRGBTexture(sys.planets[i].texture);
    }
}

StarSystem* getStarSystem()
{
    return &sys;
}

void drawStarSystem()
{
    uint8_t i;

    glBindTexture(GL_TEXTURE_2D, sunTexture);
    for(i = 0; i < sys.numStars; i++)
    {
        glPushMatrix();
        glTranslatef(sys.stars[i].position.x, sys.stars[i].position.y, sys.stars[i].position.z);
        if(sys.stars[i].size != 1.0f)
        {
            glScalef(sys.stars[i].size, sys.stars[i].size, sys.stars[i].size);
        }
        glCallList(planetMesh);
        glPopMatrix();
    }

    for(i = 0; i < sys.numPlanets; i++)
    {
        glPushMatrix();
        glTranslatef(sys.planets[i].position.x, sys.planets[i].position.y, sys.planets[i].position.z);
        if(sys.planets[i].size != 1.0f)
        {
            glScalef(sys.planets[i].size, sys.planets[i].size, sys.planets[i].size);
        }
        glBindTexture(GL_TEXTURE_2D, sys.planets[i].texture);
        glCallList(planetMesh);
        if(sys.planets[i].hasRing)
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
    glTranslatef(sys.station.position.x, sys.station.position.y, sys.station.position.z);
    glScalef(5, 5, 5);
    glCallList(stationMesh);
    glPopMatrix();
}

uint8_t hasDockingDistance(vec3* pos)
{
    if(distance3d(pos, &sys.station.dockingPosition) < 2)
    {
        return 1;
    }
    return 0;
}

vec3 getExitPosition()
{
    return sys.station.exitPosition;
}

vec3 getStationPosition()
{
    return sys.station.position;
}