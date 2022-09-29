#include "ui.h"

#include "../engine/model.h"
#include "../engine/image.h"
#include "../engine/includes/3dMath.h"
#include "../cargo.h"
#include "../universe/generator.h"
#include "../universe/satellites.h"

#include "uiutils.h"

GLuint mainTexture;
GLuint firingTexture;
GLuint stationUITexture;

//Map
uint8_t mapScrollX;
uint8_t mapScrollY;

void initUI()
{
    initPNG();
    mainTexture = loadRGBTexture("res/UI/main.png");
    firingTexture = loadRGBTexture("res/UI/firing.png");
    stationUITexture = loadRGBTexture("res/UI/StationUI.png");
}

void quitUI()
{
    quitPNG();
    deleteRGBTexture(mainTexture);
    deleteRGBTexture(firingTexture);
    deleteRGBTexture(stationUITexture);
}

void drawRadarDot(vec3 playerPos, quat playerRot, vec3 target, uint8_t color)
{
    //Project vector from player to target to 2d (z component is before/behind player)
    //Also rotate it properly
    vec3 diff = subv3(playerPos, target);
    quat qr = multQuat(QUAT_INITIAL, inverseQuat(playerRot));
    vec3 rot = multQuatVec3(qr, diff);
    rot = normalizev3(rot);

    if(rot.z > 0)
    {
        rot.x = -rot.x * 30;
        rot.y = -rot.y * 30;
    }
    else
    {
        float angle = atan2f(-rot.y, -rot.x);
        rot.x = 32 * cosf(angle);
        rot.y = 32 * sinf(angle);
    }

    float texY1 = PTC(4 + color * 4);
    float texY2 = PTC(7 + color * 4);
    drawTexQuad(119.5f + rot.x - 2, 36.5f + rot.y - 2, 4, 4, UITH, PTC(252), texY1, 1, texY2);
}

void drawUI(bool onStation, Player* player, Npc npcs[], vec3 stationPos, uint8_t autodockPossible)
{
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, mainTexture);
    glBegin(GL_QUADS);
    //Draw main UI background
    drawTexQuad(0, 0, 240, 240, UIBH, 0, 0, PTC(240), PTC(240));
    uint8_t speedTemp = (player->ship.speed / shipTypes[player->ship.type].maxSpeed) * 32;
    if(speedTemp > 0)
    {
        drawTexQuad(170, 26, speedTemp * 2, 4, UITH, 0, PTC(249), PTC(2) * speedTemp, PTC(251));
    }

    int8_t turnXTemp = (player->ship.turnSpeedX / shipTypes[player->ship.type].maxTurnSpeed) * 30 + 30;
    drawTexQuad(170 + turnXTemp, 56, 4, 4, UITH, PTC(252), 0, 1, PTC(3));

    int8_t turnYTemp = (player->ship.turnSpeedY / shipTypes[player->ship.type].maxTurnSpeed) * 30 + 30;
    drawTexQuad(170 + turnYTemp, 41, 4, 4, UITH, PTC(252), 0, 1, PTC(3));

    uint8_t shieldsTemp = (player->ship.shields / shipTypes[player->ship.type].maxShields) * 32;
    if(player->ship.shields > 0 && shieldsTemp > 0)
    {
        drawTexQuad(7, 57, shieldsTemp * 2, 4, UITH, 0, PTC(253), PTC(2) * shieldsTemp, 1);
    }

    uint8_t energyTemp = (player->ship.energy / shipTypes[player->ship.type].maxEnergy) * 32;
    if(energyTemp > 0)
    {
        drawTexQuad(7, 42, energyTemp * 2, 4, UITH, 0, PTC(253), PTC(2) * energyTemp, 1);
    }

    uint8_t fuelTemp = ((float) player->fuel / MAX_FUEL) * 32;
    if(fuelTemp > 0)
    {
        drawTexQuad(7, 27, fuelTemp * 2, 4, UITH, 0, PTC(246), PTC(2) * fuelTemp, PTC(248));
    }

    //Damage indicator
    if(player->ship.damaged)
    {
        player->ship.damaged++;
        if(player->ship.damaged > 25) //25 frames @ 50fps, ~1/2 second
        {
            player->ship.damaged = 0;
        }
        drawTexQuad(229, 11, 4, 4, UITH, PTC(252), PTC(20), 1, PTC(23));
    }

    //Fuel scoop indicator
    if(player->fuelScoopsActive)
    {
        drawTexQuad(185, 11, 4, 4, UITH, PTC(252), PTC(28), 1, PTC(31));
    }

    //Radar
    if(!onStation)
    {
        if(autodockPossible)
        {
            drawTexQuad(171, 11, 4, 4, UITH, PTC(252), PTC(24), 1, PTC(27));
        }

        drawRadarDot(player->ship.position, player->ship.rotation, stationPos, 1);

        for(uint8_t i = 0; i < NUM_NORM_NPCS; i++)
        {
            if(npcs[i].ship.type != SHIP_TYPE_NULL)
            {
                if(distance3d(&player->ship.position, &npcs[i].ship.position) < RADAR_RANGE)
                {
                    drawRadarDot(player->ship.position, player->ship.rotation, npcs[i].ship.position, 0);
                }
            }
        }
        //Contract ship
        if(npcs[NPC_CONTRACT].ship.type != SHIP_TYPE_NULL)
        {
            drawRadarDot(player->ship.position, player->ship.rotation, npcs[NPC_CONTRACT].ship.position, 3);
        }
        //Next contract satellite to visit
        else if(hasSatellites() && !checkAllSatellitesVisited())
        {
            drawRadarDot(player->ship.position, player->ship.rotation, getSatellitePosition(), 3);
        }
    }

    //Draw effect if firing
    if(player->ship.weapon.timer)
    {
        glBindTexture(GL_TEXTURE_2D, firingTexture);
        drawTexQuad(2, 72, 237, 167, UIBH, 0, 0, PTC(235), PTC(165));
    }

    glEnd();
}

void drawSaveLoadUI(uint8_t cursor)
{
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, stationUITexture);
    glBegin(GL_QUADS);
    drawTexQuad(0, 0, 240, 240, UIBH, 0, 0, PTC(240), PTC(240));
    //Save/Load icons
    drawTexQuad(CENTER(13), 200, 8, 8, UITH, PTC(241), 0, PTC(249), PTC(8));
    drawTexQuad(CENTER(13), 184, 8, 8, UITH, PTC(249), 0, PTC(256), PTC(8));
    glEnd();
    glDrawText("Save & Load", CENTER(11), 2, 0xFFFFFF);

    if(cursor == 0)
    {
        glDrawText("Save game", CENTER(9), 32, 0x00FFFF);
        glDrawText("Load game", CENTER(9), 48, 0xFFFFFF);
    }
    else
    {
        glDrawText("Save game", CENTER(9), 32, 0xFFFFFF);
        glDrawText("Load game", CENTER(9), 48, 0x00FFFF);
    }

    glDrawText("Trading", 240 - 7 * 8 - 12, 240 - 10, 0xFFFFFF);
}

void drawTradingUI(uint8_t cursor, CargoHold* playerHold, CargoHold* stationHold, SystemInfo* info)
{
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, stationUITexture);
    glBegin(GL_QUADS);
    drawTexQuad(0, 0, 240, 240, UIBH, 0, 0, PTC(240), PTC(240));
    glEnd();
    glDrawText("Trading", CENTER(7), 2, 0xFFFFFF);

    char buffer[29];
    char name[15];
    char type[4];

    glDrawText("ITEM        UNIT PRICE    QTY", 4, 16, 0xFFFFFF);
    for(uint8_t i = 0; i < NUM_CARGO_TYPES; i++)
    {
        printNameForCargo(name, i);
        printUnitForCargo(type, i);
        sprintf(buffer, "%-13s%3s %5d  %2d|%2d", name, type, getPriceForCargo(i, info),
                                                    stationHold->cargo[i], playerHold->cargo[i]);
        if(i == cursor)
        {
            glDrawText(buffer, 4, 24 + i * 8, 0x00FFFF);
        }
        else
        {
            glDrawText(buffer, 4, 24 + i * 8, 0xFFFFFF);
        }
    }

    sprintf(buffer, "%d credits", playerHold->money);
    glDrawText(buffer, CENTER(strlen(buffer)), 218, 0xFFFFFF);

    glDrawText("Save & Load", 12, 240 - 10, 0xFFFFFF);
    glDrawText("Equip ship", 240 - 10 * 8 - 12, 240 - 10, 0xFFFFFF);
}

void drawContract(Contract* contract, uint8_t cursor, uint8_t numContracts)
{
    glBegin(GL_QUADS);
    drawTexQuad(4, 194, 32, 32, UITH, PTC(240), PTC(16 + contract->type * 16),
                                        1, PTC(31 + contract->type * 16));
    glEnd();
    char buffer[29];
    if(cursor)
    {
        sprintf(buffer, "%d/%d", cursor, numContracts);
        glDrawText(buffer, 8, 48, 0xFFFFFF);
    }
    else
    {
        glDrawText("ACT", 8, 48, 0x00FF00);
    }

    glDrawText(contractTypes[contract->type], 40, 16, 0xFFFFFF);
    sprintf(buffer, "Employer: %s\n          %s", contractFirstnames[contract->employerFirstname],
                                                    contractLastnames[contract->employerLastname]);
    glDrawText(buffer, 40, 32, 0xFFFFFF);
    sprintf(buffer, "Pay: %d credits", contract->pay);
    glDrawText(buffer, 40, 56, 0xFFFFFF);
    glDrawText("Destination system:", 40, 72, 0xFFFFFF);

    SystemBaseData sbd;
    generateSystemBaseData(&sbd, getSeedForSystem(contract->targetSystem[0], contract->targetSystem[1]));
    glDrawText(sbd.info.name, 40, 80, 0xFFFFFF);

    glDrawText("Objective:", 40, 96, 0xFFFFFF);
    printObjective(buffer, contract);
    glDrawText(buffer, 40, 104, 0xFFFFFF);
}

void drawContractUI(uint8_t cursor, Contract* activeContract, Contract* contracts, uint8_t numContracts)
{
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, stationUITexture);
    glBegin(GL_QUADS);
    drawTexQuad(0, 0, 240, 240, UIBH, 0, 0, PTC(240), PTC(240));
    glEnd();
    glDrawText("Contracts", CENTER(9), 2, 0xFFFFFF);

    if(activeContract->type != CONTRACT_TYPE_NULL)
    {
        drawContract(activeContract, 0, numContracts);
    }
    else
    {
        drawContract(&contracts[cursor], cursor + 1, numContracts);
    }

    glDrawText("Equip ship", 12, 240 - 10, 0xFFFFFF);
}

void drawGameOverScreen()
{
    glLoadIdentity();
    glBegin(GL_QUADS);
    glDrawText("GAME OVER", CENTER(9), 90, 0xFFFFFF);
}

void moveCursorDown(uint8_t* i, uint8_t max)
{
    (*i)++;
    (*i) %= (max + 1); 
}

void moveCursorUp(uint8_t* i, uint8_t max)
{
    if(*i > 0)
    {
        (*i)--;
    }
    else
    {
        (*i) = max;
    }
}