#include "starsystem.h"
#include "../engine/model.h"
#include "../engine/image.h"

StarSystem sys;

GLuint planetMesh;
GLuint earthTexture;
GLuint sunTexture;

GLuint stationMesh;
GLuint stationTexture;

void initStarSystem()
{
    planetMesh = loadModelList("res/obj/Planet.obj");
    earthTexture = loadRGBTexture("res/tex/Earth.png");
    sunTexture = loadRGBTexture("res/tex/Sun.png");

    stationMesh = loadModelList("res/obj/SpaceStation.obj");
    stationTexture = loadRGBTexture("res/tex/SpaceStation.png");
}

StarSystem* getStarSystem()
{
    return &sys;
}

void loadStarSystem()
{
    //Testing with no generation
    sys.stars[0].size = 50.0f;
    sys.stars[0].position.x = 0;
    sys.stars[0].position.y = 0;
    sys.stars[0].position.z = -150;
    sys.numStars = 1;
    sys.planets[0].size = 10.0f;
    sys.planets[0].position.x = 10;
    sys.planets[0].position.y = 0;
    sys.planets[0].position.z = -20;
    sys.planets[1].size = 0.3f;
    sys.planets[1].position.x = 0;
    sys.planets[1].position.y = 0;
    sys.planets[1].position.z = 10;
    sys.numPlanets = 2;
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

    glBindTexture(GL_TEXTURE_2D, earthTexture);
    for(i = 0; i < sys.numPlanets; i++)
    {
        glPushMatrix();
        glTranslatef(sys.planets[i].position.x, sys.planets[i].position.y, sys.planets[i].position.z);
        if(sys.planets[i].size != 1.0f)
        {
            glScalef(sys.planets[i].size, sys.planets[i].size, sys.planets[i].size);
        }
        glCallList(planetMesh);
        glPopMatrix();
    }

    glBindTexture(GL_TEXTURE_2D, stationTexture);
    glPushMatrix();
    glTranslatef(sys.station.position.x, sys.station.position.y, sys.station.position.z);
    glScalef(5, 5, 5);
    glCallList(stationMesh);
    glPopMatrix();
}

uint8_t hasDockingDistance(Ship* ship)
{
    if(distance3d(&ship->position, &sys.station.dockingPosition) < 2)
    {
        return 1;
    }
    return 0;
}