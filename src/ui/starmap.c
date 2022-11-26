#include "starmap.h"

#include "../engine/model.h"
#include "../engine/image.h"
#include "../engine/camera.h"

#include "../universe/generator.h"
#include "../universe/universe.h"

#include "uiutils.h"

GLuint starMesh;
GLuint starTexture;

GLuint systemInfoTexture;

static GLuint uiTexture;

static int8_t cursor[2];
static int8_t cursorNew[2];
static float lerpTemp = 0;

void initStarmap(GLuint uiTex)
{
    uiTexture = uiTex;

    starMesh = loadModelList("res/obj/Planet.obj");
    starTexture = loadRGBTexture("res/tex/Sun.png");
}

void quitStarmap()
{
    glDeleteList(starMesh);
    deleteRGBTexture(starTexture);
}

#define CAMERA_HEIGHT 15

void calcStarmap(uint32_t ticks)
{
    vec2 cursorSystem = generateSystemPos(getSeedForSystem(cursor[0], cursor[1]), cursor[0], cursor[1]);
    vec3 cameraPos = {cursorSystem.x, CAMERA_HEIGHT, cursorSystem.y};

    if(cursorNew[0] != cursor[0] || cursorNew[1] != cursor[1])
    {
        cursorSystem = generateSystemPos(getSeedForSystem(cursorNew[0], cursorNew[1]), cursorNew[0], cursorNew[1]);
        vec3 newCameraPos = {cursorSystem.x, CAMERA_HEIGHT, cursorSystem.y};

        lerpTemp += ticks / 500.0f;
        //Prevent overruns
        if(lerpTemp > 1)
        {
            lerpTemp = 1;
        }
        cameraPos = lerpv3(cameraPos, newCameraPos, lerpTemp);

        if(distance3d(&cameraPos, &newCameraPos) < 0.1f)
        {
            cursor[0] = cursorNew[0];
            cursor[1] = cursorNew[1];
            lerpTemp = 0;
        }
    }

    setCameraPos(cameraPos);
    setCameraRot(quatFromAngles((vec3) {-M_PI_2, 0, 0}));
}

#define STAR_DIST 1.5f

vec2 starDistSinCos(float angle)
{
    return (vec2) {sinf(angle) * STAR_DIST,
                   cosf(angle) * STAR_DIST};
}

void drawCircle(float radius)
{
    glBegin(GL_LINES);
    for(float angle = 0; angle <= 2 * M_PI; angle += 0.2f)
    {
        glVertex3f(sinf(angle) * radius, 0, cosf(angle) * radius);
    }
    glEnd();
}

void drawStarmap3d(uint8_t* currentSystem, float fuel, uint8_t contractSystem[2], bool contractActive)
{
    glBindTexture(GL_TEXTURE_2D, starTexture);

    uint8_t xmin = cursor[0] < 2 ? 0 : cursor[0] - 2;
    uint8_t xmax = cursor[0] > UNIVERSE_SIZE - 3 ? UNIVERSE_SIZE - 1 : cursor[0] + 2;

    uint8_t ymin = cursor[1] < 2 ? 0 : cursor[1] - 2;
    uint8_t ymax = cursor[1] > UNIVERSE_SIZE - 3 ? UNIVERSE_SIZE - 1 : cursor[1] + 2;

    for(uint8_t i = xmin; i < xmax + 1; i++)
    {
        for(uint8_t j = ymin; j < ymax + 1; j++)
        {
            uint8_t numStars = getNumStarsForSystem(getSeedForSystem(i, j));
            vec2 systemPos = generateSystemPos(getSeedForSystem(i, j), i, j);

            glPushMatrix();
            switch(numStars)
            {
                case 1:
                {
                    glTranslatef(systemPos.x, 0, systemPos.y);
                    glCallList(starMesh);
                    break;
                }
                case 2:
                {
                    vec2 sc = starDistSinCos(randf(2 * M_PI));
                    glTranslatef(systemPos.x - sc.x, 0, systemPos.y - sc.y);
                    glCallList(starMesh);
                    glTranslatef(sc.x * 2, 0, sc.y * 2);
                    glCallList(starMesh);
                    break;
                }
                case 3:
                {
                    //The 3 positions are calculated based on an angle:
                    //sin(a), cos(a)
                    //sin(a + 2/3pi), cos(a + 2/3pi)
                    //sin(a + 4/3pi), cos(a + 4/3pi)

                    float angle = randf(2 * M_PI);
                    vec2 sc = starDistSinCos(angle);
                    glTranslatef(systemPos.x + sc.x, 0, systemPos.y + sc.y);
                    glCallList(starMesh);
                    glTranslatef(-sc.x, 0, -sc.y);

                    angle += 2.0f / 3.0f * M_PI;
                    sc = starDistSinCos(angle);
                    glTranslatef(sc.x, 0, sc.y);
                    glCallList(starMesh);
                    glTranslatef(-sc.x, 0, -sc.y);

                    angle += 2.0f / 3.0f * M_PI;
                    sc = starDistSinCos(angle);
                    glTranslatef(sc.x, 0, sc.y);
                    glCallList(starMesh);
                    break;
                }
            }
            glPopMatrix();
        }
    }
    
    //Draw range circle
    glPushMatrix();
    vec2 currentPos = generateSystemPos(getSeedForSystem(currentSystem[0], currentSystem[1]), currentSystem[0], currentSystem[1]);
    glTranslatef(currentPos.x, 0, currentPos.y);
    drawCircle(fuel);
    glPopMatrix();

    //Draw contract system circle
    if(contractActive)
    {
        uint8_t numStars = getNumStarsForSystem(getSeedForSystem(contractSystem[0], contractSystem[1]));
        vec2 systemPos = generateSystemPos(getSeedForSystem(contractSystem[0], contractSystem[1]), contractSystem[0], contractSystem[1]);
        glTranslatef(systemPos.x, 0, systemPos.y);
        glColor3f(1, 1, 0);
        drawCircle(3);
        glColor3f(1, 1, 1);
    }
}

const char* governmentLevels[MAX_GOVERNMENT] = {
    "Anarchy",
    "Feudal",
    "Dictatorship",
    "Corporate state",
    "Democracy"
};

void drawSystemInfoBox()
{
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, uiTexture);
    glBegin(GL_QUADS);
    //Background
    drawTexQuad(0, 0, 240, 72, UIBH, 0, PTC(12), PTC(239), PTC(83));
    //Content
    SystemBaseData sbd;
    generateSystemBaseData(&sbd, getSeedForSystem(cursor[0], cursor[1]));

    char buffer[29];
    glDrawText(sbd.info.name, CENTER(strlen(sbd.info.name)), 168 + 4, TEXT_WHITE);

    glDrawText("Tech level:", 8, 168 + 4 + 10, TEXT_GREEN);
    sprintf(buffer, "%d", sbd.info.techLevel);
    glDrawText(buffer, 8 + 96, 168 + 4 + 10, TEXT_WHITE);

    glDrawText("Government:", 8, 168 + 4 + 18, TEXT_GREEN);
    sprintf(buffer, "%s", governmentLevels[sbd.info.government]);
    glDrawText(buffer, 8 + 96, 168 + 4 + 18, TEXT_WHITE);

    glDrawText("Planets:", 8, 168 + 4 + 26, TEXT_GREEN);
    for(uint8_t i = 0; i < sbd.numPlanets; i++)
    {
        drawTexQuad(80 + i * 12, 34, 8, 8, UITH,
                    PTC(240), PTC(sbd.paletteIndices[i] * 8), PTC(248), PTC(7 + sbd.paletteIndices[i] * 8));
    }

    char description[29 * 3];
    generateSystemDescription(description, &sbd);
    glDrawText(description, 8, 168 + 4 + 36, TEXT_WHITE);

    glEnd();
}

void moveStarmapCursor(int8_t dirX, int8_t dirY)
{
    if(cursorNew[0] != cursor[0] || cursorNew[1] != cursor[1])
    {
        return;
    }

    cursorNew[0] += dirX;
    if(cursorNew[0] < 0)
    {
        cursorNew[0] = 0;
    }
    else if(cursorNew[0] >= UNIVERSE_SIZE)
    {
        cursorNew[0] = UNIVERSE_SIZE - 1;
    }

    cursorNew[1] += dirY;
    if(cursorNew[1] < 0)
    {
        cursorNew[1] = 0;
    }
    else if(cursorNew[1] >= UNIVERSE_SIZE)
    {
        cursorNew[1] = UNIVERSE_SIZE - 1;
    }
}

void setStarmapCursor(int8_t x, int8_t y)
{
    cursor[0] = x;
    cursor[1] = y;
    cursorNew[0] = x;
    cursorNew[1] = y;
}

int8_t* getStarmapCursor()
{
    return cursor;
}