#include "satellites.h"
#include "../engine/model.h"
#include "../engine/image.h"

Satellite satellites[NUM_SATELLITES];
uint8_t satelliteIndex;
uint8_t satelliteVisitIndex;

GLuint satelliteMesh;
GLuint satelliteTexture;

void initSatellites()
{
    satelliteMesh = loadModelList("res/obj/Satellite.obj");
    satelliteTexture = loadRGBTexture("res/tex/Satellite.png");
    satelliteIndex = 0;
    satelliteVisitIndex = 0;
}

void createSatellite(vec3 pos, float rotY)
{
    if(satelliteIndex < NUM_SATELLITES)
    {
        satellites[satelliteIndex].position = pos;
        satellites[satelliteIndex++].rotationY = rotY;
    }
}

void clearSatellites()
{
    satelliteIndex = 0;
    satelliteVisitIndex = 0;
}

void drawSatellites()
{
    glBindTexture(GL_TEXTURE_2D, satelliteTexture);
    for(uint8_t i = 0; i < satelliteIndex; i++)
    {
        glPushMatrix();
        glTranslatef(satellites[i].position.x, satellites[i].position.y, satellites[i].position.z);
        glCallList(satelliteMesh);
        glPopMatrix();
    }
}

void checkVisitSatellite(vec3* playerPos)
{
    if(satelliteVisitIndex < satelliteIndex && distance3d(playerPos, &satellites[satelliteVisitIndex].position) < SATELLITE_VISIT_DISTANCE)
    {
        satelliteVisitIndex++;
    }
}

uint8_t checkAllSatellitesVisited()
{
    return satelliteVisitIndex == satelliteIndex;
}

vec3 getSatellitePosition()
{
    if(satelliteVisitIndex < satelliteIndex)
    {
        return satellites[satelliteVisitIndex].position;
    }
    return (vec3){.x = 0, .y = 0, .z = 0};
}

uint8_t hasSatellites()
{
    return satelliteIndex;
}