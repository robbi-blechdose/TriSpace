#include "contractui.h"

#include "../engine/image.h"
#include "../engine/model.h"
#include "../engine/camera.h"

#include "uiutils.h"

#include "../universe/generator/generator.h"

static int8_t cursor = 0;

static GLuint uiTexture;

static GLuint satelliteMesh;
static float satelliteRotation = 0;

static Ship playerShip;
static Ship policeShip;
static Ship cruiseShip;

static uint32_t firingTicks = 0;

#define NUM_WAYPOINTS 4
static vec3 routeWaypoints[NUM_WAYPOINTS] = {
    {-25, 0,  -2},
    { -5, 0,   2},
    {  5, 0, -14},
    { 25, 0,  -2}
};

static vec3 satelliteWaypoints[NUM_WAYPOINTS] = {
    {-25, 0,  -2},
    { -5, 0, -14},
    {  5, 0,   2},
    { 25, 0,  -2}
};

static uint8_t waypointIndex;
static vec3* waypoints;

void initContractUI(GLuint uiTex)
{
    uiTexture = uiTex;

    satelliteMesh = loadModelList("res/obj/Satellite.obj");
    
    waypoints = routeWaypoints;

    //Init ships
    playerShip = (Ship) {.position = waypoints[0],
                         .rotation = QUAT_INITIAL,
                         .type = SHIP_TYPE_PLAYER};
    policeShip = (Ship) {.position = (vec3) {-12, 0, -8},
                         .rotation = QUAT_INITIAL,
                         .type = SHIP_TYPE_POLICE};
    cruiseShip = (Ship) {.position = (vec3) {12, 0, -8},
                         .rotation = QUAT_INITIAL,
                         .type = SHIP_TYPE_CRUISELINER};
}

void quitContractUI()
{
    glDeleteList(satelliteMesh);
}

void movePlayerShipAlongWaypoints(uint32_t ticks)
{
    turnShipTowardsPoint(&playerShip, waypoints[waypointIndex]);
    accelerateShipLimit(&playerShip, 1, ticks, 0.5f);
    calcShip(&playerShip, ticks);

    if(distance3d(&playerShip.position, &waypoints[waypointIndex]) < 0.2f)
    {
        waypointIndex++;
    }
    if(waypointIndex == NUM_WAYPOINTS)
    {
        waypointIndex = 0;
        playerShip.position = waypoints[0];
    }
}

void calcContractUIInternal(uint32_t ticks, Contract* contract)
{
    setCameraPos((vec3) {0, 15, 15});
    setCameraRot(quatFromAngles((vec3) {DEG_TO_RAD(-45), 0, 0}));

    switch(contract->type)
    {
        case CONTRACT_GET_ITEM:
        {
            movePlayerShipAlongWaypoints(ticks);
            break;
        }
        case CONTRACT_SMUGGLE:
        {
            movePlayerShipAlongWaypoints(ticks);
            turnShipTowardsPoint(&policeShip, playerShip.position);
            calcShip(&policeShip, ticks);
            break;
        }
        case CONTRACT_DESTROY_SHIP:
        {
            turnShipTowardsPoint(&playerShip, cruiseShip.position);
            calcShip(&playerShip, ticks);

            firingTicks += ticks;
            if(firingTicks > 1200)
            {
                fireWeapons(&playerShip);
                playerShip.weapon.distanceToHit = distance3d(&playerShip.position, &cruiseShip.position);
                firingTicks = 0;
            }

            break;
        }
        case CONTRACT_RECONNAISSANCE:
        {
            movePlayerShipAlongWaypoints(ticks);

            satelliteRotation += (45 * ticks) / 1000.0f;
            //Clamp values to keep accuracy even when we rotate for a long time
            if(satelliteRotation >= 360)
            {
                satelliteRotation -= 360;
            }

            break;
        }
    }
}

void drawRoute()
{
    //Draw route
    glColor3f(0, 1, 1);
    glBegin(GL_LINES);
    for(uint8_t i = 0; i < NUM_WAYPOINTS - 1; i++)
    {
        vec3 dist = subv3(waypoints[i + 1], waypoints[i]);
        #define NUM_STRIPS 16
        for(uint8_t j = 0; j < NUM_STRIPS; j++)
        {
            glVertex3f(waypoints[i].x + dist.x / NUM_STRIPS * j,
                       waypoints[i].y + dist.y / NUM_STRIPS * j,
                       waypoints[i].z + dist.z / NUM_STRIPS * j);
        }
    }
    glEnd();
    glColor3f(1, 1, 1);
}

void drawContractUI3dInternal(Contract* contract)
{
    //Draw as wireframe
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    switch(contract->type)
    {
        case CONTRACT_GET_ITEM:
        {
            drawRoute();
            break;
        }
        case CONTRACT_SMUGGLE:
        {
            drawRoute();

            //Draw police ship
            glColor3f(1, 0, 0);
            drawShip(&policeShip);
            glColor3f(1, 1, 1);
            break;
        }
        case CONTRACT_DESTROY_SHIP:
        {
            //Draw cruise ship
            glColor3f(1, 1, 0);
            drawShip(&cruiseShip);
            glColor3f(1, 1, 1);
            break;
        }
        case CONTRACT_RECONNAISSANCE:
        {
            drawRoute();

            //Draw satellites
            for(uint8_t i = 1; i < NUM_WAYPOINTS - 1; i++) //Skip first and last waypoint (offscreen)
            {
                if(waypointIndex > i)
                {
                    glColor3f(0, 1, 0);
                }

                glPushMatrix();
                glTranslatef(waypoints[i].x, waypoints[i].y, waypoints[i].z);
                glRotatef(satelliteRotation, 0, 1, 0);
                glCallList(satelliteMesh);
                glPopMatrix();

                if(waypointIndex > i)
                {
                    glColor3f(1, 1, 1);
                }
            }
            break;
        }
    }

    drawShip(&playerShip);

    //Back to normal
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
}

void calcContractUI(uint32_t ticks, Contract* activeContract, Contract* contracts)
{
    if(activeContract->type != CONTRACT_TYPE_NULL)
    {
        calcContractUIInternal(ticks, activeContract);
    }
    else
    {
        calcContractUIInternal(ticks, &contracts[cursor]);
    }
}

void drawContractUI3d(Contract* activeContract, Contract* contracts)
{
    if(activeContract->type != CONTRACT_TYPE_NULL)
    {
        drawContractUI3dInternal(activeContract);
    }
    else
    {
        drawContractUI3dInternal(&contracts[cursor]);
    }
}

void drawContract(Contract* contract)
{
    char buffer[29];

    glDrawText("Employer:", 2, 112, TEXT_GREEN);
    sprintf(buffer, "%s %s", contractFirstnames[contract->employerFirstname], contractLastnames[contract->employerLastname]);
    glDrawText(buffer, 18, 120, TEXT_WHITE);

    glDrawText("Pay:", 2, 136, TEXT_GREEN);
    sprintf(buffer, "%d credits", contract->pay);
    glDrawText(buffer, 42, 136, TEXT_WHITE);

    glDrawText("Destination system:", 2, 152, TEXT_GREEN);
    StarSystemInfo info;
    generateStarSystemInfo(&info, getSeedForSystem(contract->targetSystem[0], contract->targetSystem[1]));
    glDrawText(info.name, 18, 160, TEXT_WHITE);

    glDrawText("Objective:", 2, 176, TEXT_GREEN);
    printObjective(buffer, contract);
    glDrawText(buffer, 18, 184, TEXT_WHITE);
}

void drawContractUI(Contract* activeContract, Contract* contracts, uint8_t numContracts)
{
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, uiTexture);
    glBegin(GL_QUADS);
    drawTexQuad(0, 240 - 12, 240, 12, UIBH, 0, 0, PTC(239), PTC(11));
    drawTexQuad(0, 0, 240, 130, UIBH, 0, PTC(12), PTC(239), PTC(141));
    glEnd();
    glDrawText("Contracts", CENTER(9), 2, TEXT_DKGREY);

    if(activeContract->type != CONTRACT_TYPE_NULL)
    {
        drawContract(activeContract);

        glDrawText("ACT", 214, 112, TEXT_WHITE);
    }
    else
    {
        drawContract(&contracts[cursor]);

        char buffer[29];
        sprintf(buffer, "%d/%d", cursor + 1, numContracts);
        glDrawText(buffer, 214, 112, TEXT_WHITE);
    }

    glDrawText("Equip ship", 12, 240 - 10, TEXT_DKGREY);
}

void moveContractUICursor(int8_t dir, Contract* contracts, uint8_t numContracts)
{
    cursor += dir;
    if(cursor < 0)
    {
        cursor = numContracts - 1;
    }
    else if(cursor >= numContracts)
    {
        cursor = 0;
    }

    switch(contracts[cursor].type)
    {
        case CONTRACT_GET_ITEM:
        {
            waypoints = routeWaypoints;
            playerShip.position = waypoints[0];
            break;
        }
        case CONTRACT_SMUGGLE:
        {
            waypoints = routeWaypoints;
            playerShip.position = waypoints[0];
            break;
        }
        case CONTRACT_DESTROY_SHIP:
        {
            playerShip.position = (vec3) {-16, 0, -2};
            playerShip.speed = 0;
            break;
        }
        case CONTRACT_RECONNAISSANCE:
        {
            waypoints = satelliteWaypoints;
            playerShip.position = waypoints[0];
            break;
        }
    }
    waypointIndex = 0;
}

int8_t getContractUICursor()
{
    return cursor;
}

void setContractUICursor(int8_t value)
{
    cursor = value;
}