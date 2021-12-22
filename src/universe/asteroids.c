#include "asteroids.h"
#include "../engine/model.h"
#include "../engine/image.h"

GLuint asteroidMesh;
GLuint asteroidTexture;

Asteroid asteroids[NUM_ASTEROIDS];

void initAsteroids()
{
    asteroidMesh = loadModelList("res/obj/Asteroid.obj");
    asteroidTexture = loadRGBTexture("res/tex/Asteroid.png");
}

void drawAsteroids()
{
    glBindTexture(GL_TEXTURE_2D, asteroidTexture);
    for(uint8_t i = 0; i < NUM_ASTEROIDS; i++)
    {
        glPushMatrix();
        glTranslatef(asteroids[i].position.x, asteroids[i].position.y, asteroids[i].position.z);
        if(asteroids[i].size != 1.0f)
        {
            float size = asteroids[i].size;
            glScalef(size, size, size);
        }
        glCallList(asteroidMesh);
        glPopMatrix();
    }
}

void damageAsteroid(uint8_t index)
{
    //TODO
}