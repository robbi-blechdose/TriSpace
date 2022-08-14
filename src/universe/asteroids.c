#include "asteroids.h"
#include "../engine/model.h"
#include "../engine/image.h"
#include "../engine/effects.h"

GLuint asteroidMesh;
GLuint asteroidTexture;

Asteroid asteroids[NUM_ASTEROIDS];
vec3 asteroidFieldPos;

void initAsteroids()
{
    asteroidMesh = loadModelList("res/obj/Asteroid.obj");
    asteroidTexture = loadRGBTexture("res/tex/Asteroid.png");
}

void createAsteroid(uint8_t i)
{
    asteroids[i].size = 0.5f + randf(4.5f);
    //TODO: Improve
    asteroids[i].position.x = asteroidFieldPos.x + (randf(ASTEROID_FIELD_SIZE) - ASTEROID_FIELD_SIZE / 2);
    asteroids[i].position.y = asteroidFieldPos.y + (randf(ASTEROID_FIELD_SIZE) - ASTEROID_FIELD_SIZE / 2);
    asteroids[i].position.z = asteroidFieldPos.z + (randf(ASTEROID_FIELD_SIZE) - ASTEROID_FIELD_SIZE / 2);
    asteroids[i].health = ASTEROID_HEALTH;
}

void createAsteroids(vec3 pos)
{
    asteroidFieldPos = pos;
    for(uint8_t i = 0; i < NUM_ASTEROIDS; i++)
    {
        createAsteroid(i);
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

bool checkAsteroidHit(vec3* position, vec3* direction, float damage, uint8_t mineChance)
{
    for(uint8_t i = 0; i < NUM_ASTEROIDS; i++)
    {
        float hit = checkHitSphere(position, direction, &asteroids[i].position, asteroids[i].size);
        if(hit != -1)
        {
            asteroids[i].health -= damage;
            if(asteroids[i].health <= 0)
            {
                createEffect(asteroids[i].position, EXPLOSION);
                //Asteroid was destroyed, spawn a new one
                createAsteroid(i);
                if(randr(100) < mineChance)
                {
                    //We got mineable resources here
                    return true;
                }
            }
            else
            {
                //createEffect(asteroids[i].position, SPARKS);
            }
            break;
        }
    }
    return false;
}

Asteroid* getAsteroids()
{
    return asteroids;
}