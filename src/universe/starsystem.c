#include "starsystem.h"
#include "../engine/model.h"
#include "../engine/image.h"

GLuint planetMesh;
GLuint planetRingMesh;
GLuint planetRingTexture;
GLuint sunTextures[NUM_SUN_TYPES];
GLuint stationMesh;
GLuint stationTexture;

void initStarSystem()
{
    planetMesh = loadModelList("res/obj/Planet.obj");
    planetRingMesh = loadModelList("res/obj/Planet_Ring.obj");
    planetRingTexture = loadRGBTexture("res/tex/Planet_Ring.png");
    sunTextures[0] = loadRGBTexture("res/tex/suns/Normal.png");
    sunTextures[1] = loadRGBTexture("res/tex/suns/Blue.png");
    sunTextures[2] = loadRGBTexture("res/tex/suns/Red.png");

    stationMesh = loadModelList("res/obj/SpaceStation.obj");
    stationTexture = loadRGBTexture("res/tex/SpaceStation.png");
}

void quitStarSystem()
{
    glDeleteList(planetMesh);
    glDeleteList(planetRingMesh);
    deleteRGBTexture(planetRingTexture);
    deleteRGBTexture(sunTextures[0]);
    deleteRGBTexture(sunTextures[1]);

    glDeleteList(stationMesh);
    deleteRGBTexture(stationTexture);
}

void deleteStarSystem(StarSystem* system)
{
    for(uint8_t i = 0; i < system->info.numPlanets; i++)
    {
        deleteRGBTexture(system->planets[i].texture);
    }
}

void drawStarSystem(StarSystem* system)
{
    for(uint8_t i = 0; i < system->info.numStars; i++)
    {
        glPushMatrix();
        glTranslatef(system->stars[i].position.x, system->stars[i].position.y, system->stars[i].position.z);
        if(system->stars[i].size != 1.0f)
        {
            float size = system->stars[i].size;
            glScalef(size, size, size);
        }
        glBindTexture(GL_TEXTURE_2D, sunTextures[system->info.starTypes[i]]);
        glCallList(planetMesh);
        glPopMatrix();
    }

    for(uint8_t i = 0; i < system->info.numPlanets; i++)
    {
        glPushMatrix();
        glTranslatef(system->planets[i].position.x, system->planets[i].position.y, system->planets[i].position.z);
        if(system->planets[i].size != 1.0f)
        {
            float size = system->planets[i].size;
            glScalef(size, size, size);
        }
        glBindTexture(GL_TEXTURE_2D, system->planets[i].texture);
        glCallList(planetMesh);
        if(system->planets[i].hasRing)
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
    glTranslatef(system->station.position.x, system->station.position.y, system->station.position.z);
    glScalef(5, 5, 5);
    glCallList(stationMesh);
    glPopMatrix();
}

bool hasDockingDistance(vec3* pos, vec3* dockingPos)
{
    return distance3d(pos, dockingPos) < 2;
}

vec3 getRandomFreePosBounds(StarSystem* system, vec3 center, vec3 bounds, float minDistanceFromObjects, float minDistanceFromCenter)
{
    vec3 vec;
    bool ok = false;
    while(!ok)
    {
        vec.x = center.x + randf(bounds.x) - (bounds.x / 2);
        vec.y = center.y + randf(bounds.y) - (bounds.y / 2);
        vec.z = center.z + randf(bounds.z) - (bounds.z / 2);
        ok = true;

        if(distance3d(&vec, &center) < minDistanceFromCenter)
        {
            ok = false;
            continue;
        }

        for(uint8_t i = 0; i < system->info.numStars; i++)
        {
            if(distance3d(&vec, &system->stars[i].position) < system->stars[i].size + minDistanceFromObjects)
            {
                ok = false;
                break;
            }
        }
        for(uint8_t i = 0; i < system->info.numPlanets; i++)
        {
            if(distance3d(&vec, &system->planets[i].position) < system->planets[i].size + minDistanceFromObjects)
            {
                ok = false;
                break;
            }
        }
    }
    return vec;
}

vec3 getRandomFreePos(StarSystem* system, float minDistanceFromObjects)
{
    return getRandomFreePosBounds(system, (vec3) {.x = 0, .y = 0, .z = 0}, (vec3) {.x = 500, .y = 150, .z = 500}, minDistanceFromObjects, 0);
}

bool hasSunFuelDistance(StarSystem* system, vec3* pos)
{
    for(uint8_t i = 0; i < system->info.numStars; i++)
    {
        if(distance3d(pos, &system->stars[i].position) < system->stars[i].size + 5)
        {
            return true;
        }
    }
    return false;
}