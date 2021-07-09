#include "starsystem.h"
#include "engine/model.h"
#include "engine/image.h"

Star stars[3];
uint8_t numStars;
Planet planets[8];
uint8_t numPlanets;

GLuint planetMesh;
GLuint earthTexture;

void initStarSystem()
{
    planetMesh = loadModelList("res/obj/Planet.obj");
    earthTexture = loadRGBTexture("res/tex/Earth.png");
}

void loadStarSystem()
{
    //Testing with no generation
    planets[0].size = 10.0f;
    planets[0].position.x = 0;
    planets[0].position.y = 0;
    planets[0].position.z = -2;
    planets[1].size = 0.3f;
    planets[1].position.x = 0;
    planets[1].position.y = 0;
    planets[1].position.z = 10;
    numPlanets = 2;
}

void drawStarSystem()
{
    uint8_t i;
    glBindTexture(GL_TEXTURE_2D, earthTexture);
    for(i = 0; i < numPlanets; i++)
    {
        glPushMatrix();
        glTranslatef(planets[i].position.x, planets[i].position.y, planets[i].position.z);
        if(planets[i].size != 1.0f)
        {
            glScalef(planets[i].size, planets[i].size, planets[i].size);
        }
        glCallList(planetMesh);
        glPopMatrix();
    }
}