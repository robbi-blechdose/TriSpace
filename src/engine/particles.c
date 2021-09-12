#include "particles.h"
#include <stdlib.h>
#include "GL/gl.h"

#define MAX_EMITTERS 4
Particles particles[MAX_EMITTERS];
uint8_t particlesExist[MAX_EMITTERS];

void initParticleSystem()
{
    //TODO: Load texture
    //TODO: Load models
}

uint8_t createEmitter(vec3 position)
{
    for(uint8_t i = 0; i < MAX_EMITTERS; i++)
    {
        if(!particlesExist[i])
        {
            for(uint8_t j = 0; j < NUM_PARTICLES; j++)
            {
                particles[i].positions[j] = position;
            }
            //TODO: Create
            return 1;
        }
    }
    return 0;
}

void calcParticles(uint32_t steps)
{
    for(uint8_t i = 0; i < MAX_EMITTERS; i++)
    {
        if(particlesExist[i])
        {
            uint8_t alive = 0;
            for(uint8_t j = 0; j < NUM_PARTICLES; j++)
            {
                //Position change
                particles[i].positions[j] = addv3(particles[i].positions[j], particles[i].directions[j]);
                //Particle lifetime
                particles[i].lives[j] -= steps;
                if(particles[i].lives[j] > 0)
                {
                    alive = 1;
                }
            }
            if(!alive)
            {
                particlesExist[i] = 0;
            }
        }
    }
}

void drawParticles()
{
    //TODO: Bind texture
    for(uint8_t i = 0; i < MAX_EMITTERS; i++)
    {
        if(particlesExist[i])
        {
            for(uint8_t j = 0; j < NUM_PARTICLES; j++)
            {
                glPushMatrix();
                glTranslatef(particles[i].positions[j].x, particles[i].positions[j].y, particles[i].positions[j].z);
                //TODO: Draw model
                glPopMatrix();
            }
        }
    }
}