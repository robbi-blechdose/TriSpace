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
    glBindTexture(GL_TEXTURE_2D, sunTexture);
    for(uint8_t i = 0; i < starSystem->numStars; i++)
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

    for(uint8_t i = 0; i < starSystem->numPlanets; i++)
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
    return distance3d(pos, dockingPos) < 2;
}

vec3 getRandomFreePos(StarSystem* starSystem, float minDistanceFromObjects)
{
    vec3 vec;
    uint8_t ok = 0;
    while(!ok)
    {
        vec.x = randf(500) - 250;
        vec.z = randf(500) - 250;
        vec.y = randf(50) - 25;
        ok = 1;

        for(uint8_t i = 0; i < starSystem->numStars; i++)
        {
            if(distance3d(&vec, &starSystem->stars[i].position) < starSystem->stars[i].size + minDistanceFromObjects)
            {
                ok = 0;
            }
        }
        for(uint8_t i = 0; i < starSystem->numPlanets; i++)
        {
            if(distance3d(&vec, &starSystem->planets[i].position) < starSystem->planets[i].size + minDistanceFromObjects)
            {
                ok = 0;
            }
        }
    }
    return vec;
}