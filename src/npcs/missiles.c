#include "missiles.h"

#include "../fk-engine-core/model.h"
#include "../fk-engine-core/image.h"
#include "../engine/effects.h"
#include "../fk-engine-core/audio.h"

typedef struct {
    vec3 position;
    quat rotation;

    Ship* targetShip;

    int16_t flightTime;
} Missile;

#define NUM_MISSILES 8
Missile missiles[NUM_MISSILES];

#define MISSILE_FLIGHT_TIME 5000
#define MISSILE_SPEED 9.0f
#define MISSILE_HIT_RADIUS 3.0f
#define MISSILE_DAMAGE 6

GLuint missileMesh;
GLuint missileTexture;

uint8_t sampleMissile;

void initMissiles()
{
    missileMesh = loadModelList("res/obj/Missile.obj");
    missileTexture = loadRGBTexture("res/tex/Missile.png");

    sampleMissile = loadSample("res/sfx/rlauncher.wav");
}

void quitMissiles()
{
    glDeleteList(missileMesh);
    deleteRGBTexture(missileTexture);
}

void createMissile(vec3 pos, quat rot, Ship* targetShip)
{
    for(uint8_t i = 0; i < NUM_MISSILES; i++)
    {
        if(missiles[i].flightTime <= 0)
        {
            missiles[i].position = pos;
            missiles[i].rotation = rot;
            missiles[i].targetShip = targetShip;
            missiles[i].flightTime = MISSILE_FLIGHT_TIME;
            break;
        }
    }
    
    playSample(sampleMissile);
}

void calcMissiles(uint32_t ticks)
{
    for(uint8_t i = 0; i < NUM_MISSILES; i++)
    {
        if(missiles[i].flightTime)
        {
            missiles[i].rotation = quatLookAt(missiles[i].position, missiles[i].targetShip->position, (vec3) {0, 0, -1}, (vec3) {0, 1, 0});

            vec3 posDiff = multQuatVec3(missiles[i].rotation, (vec3) {0, 0, -1});
            posDiff = scalev3(MISSILE_SPEED * ticks / 1000.0f, posDiff);
            missiles[i].position = addv3(missiles[i].position, posDiff);

            missiles[i].flightTime -= ticks;

            if(distance3d(&missiles[i].targetShip->position, &missiles[i].position) < MISSILE_HIT_RADIUS)
            {
                createEffect(missiles[i].position, EXPLOSION);
                missiles[i].flightTime = 0;

                damageShip(missiles[i].targetShip, MISSILE_DAMAGE, DAMAGE_SOURCE_PLAYER); //TODO: different source?
            }
        }
    }
}

void drawMissiles()
{
    glBindTexture(GL_TEXTURE_2D, missileTexture);
    
    for(uint8_t i = 0; i < NUM_MISSILES; i++)
    {
        if(missiles[i].flightTime)
        {
            glPushMatrix();
            glTranslatef(missiles[i].position.x, missiles[i].position.y, missiles[i].position.z);

            float rot[16];
            quat temp = multQuat(quatFromAngles((vec3) {0, M_PI, 0}), inverseQuat(missiles[i].rotation));
            quatToMatrix(rot, temp);
            glMultMatrixf(rot);

            glCallList(missileMesh);
            glPopMatrix();
        }
    }
}