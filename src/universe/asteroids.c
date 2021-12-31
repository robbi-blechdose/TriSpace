#include "asteroids.h"
#include "../engine/model.h"
#include "../engine/image.h"
#include "../engine/util.h"

GLuint asteroidMesh;
GLuint asteroidTexture;

Asteroid asteroids[NUM_ASTEROIDS];
vec3 asteroidFieldPos;

void initAsteroids()
{
    asteroidMesh = loadModelList("res/obj/Asteroid.obj");
    asteroidTexture = loadRGBTexture("res/tex/Asteroid.png");
}

void createAsteroids(vec3 pos)
{
    asteroidFieldPos = pos;
    for(uint8_t i = 0; i < NUM_ASTEROIDS; i++)
    {
        asteroids[i].size = 0.5f + randf(4.5f);
        //TODO: Improve
        asteroids[i].position.x = pos.x + (randf(ASTEROID_FIELD_SIZE) - ASTEROID_FIELD_SIZE / 2);
        asteroids[i].position.y = pos.y + (randf(ASTEROID_FIELD_SIZE) - ASTEROID_FIELD_SIZE / 2);
        asteroids[i].position.z = pos.z + (randf(ASTEROID_FIELD_SIZE) - ASTEROID_FIELD_SIZE / 2);
    }
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