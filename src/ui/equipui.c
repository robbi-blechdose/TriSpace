#include "equipui.h"

#include "../fk-engine-core/image.h"
#include "../fk-engine-core/model.h"
#include "../fk-engine-core/camera.h"
#include "../fk-engine-core/text.h"

#include "../equipment.h"

#include "uiutils.h"

static int8_t cursor = 0;

static GLuint uiTexture;

GLuint equipShipMesh;
GLuint equipmentMeshes[6];

static float equipShipRot = 0;

const uint8_t equipmentTypeMeshMapping[NUM_EQUIPMENT_TYPES] = {
    [Fuel] = 0,
    [Cargo30] = 1,
    [LaserMkII] = 2,
    [LaserMkIII] = 2,
    [MiningLaser] = 2,
    [DockingComputer] = 3,
    [FuelScoops] = 4,
    [Missile] = 5
};

void initEquipUI(GLuint uiTex)
{
    uiTexture = uiTex;

    equipShipMesh = loadModelList("res/obj/ships/Ship.obj");
    equipmentMeshes[0] = loadModelList("res/obj/equipment/fuel.obj");
    equipmentMeshes[1] = loadModelList("res/obj/equipment/cargohold.obj");
    equipmentMeshes[2] = loadModelList("res/obj/equipment/weapons.obj");
    equipmentMeshes[3] = loadModelList("res/obj/equipment/dockingcomputer.obj");
    equipmentMeshes[4] = loadModelList("res/obj/equipment/fuelscoops.obj");
    equipmentMeshes[5] = loadModelList("res/obj/Missile.obj");
}

void quitEquipUI()
{
    glDeleteList(equipShipMesh);
    glDeleteList(equipmentMeshes[0]);
    glDeleteList(equipmentMeshes[1]);
    glDeleteList(equipmentMeshes[2]);
    glDeleteList(equipmentMeshes[3]);
    glDeleteList(equipmentMeshes[4]);
    glDeleteList(equipmentMeshes[5]);
}

void calcEquipUI(uint32_t ticks)
{
    equipShipRot += (45 * ticks) / 1000.0f;
    //Clamp values to keep accuracy even when we rotate for a long time
    if(equipShipRot >= 360)
    {
        equipShipRot -= 360;
    }

    setCameraPos((vec3) {0, 2.5f, 4});
    setCameraRotMat(quatFromAngles((vec3) {DEG_TO_RAD(-45), 0, 0}));
}

void drawEquipUI3d(Player* player)
{
    //Draw as wireframe
    glDisable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glRotatef(equipShipRot, 0, 1, 0);
    glCallList(equipShipMesh);

    //Draw selected equipment
    glColor3f(1, 1, 0);
    glCallList(equipmentMeshes[equipmentTypeMeshMapping[cursor]]);
    glColor3f(1, 1, 1);

    //Back to normal
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_CULL_FACE);
}

void drawEquipUI(Player* player)
{
    glLoadIdentity();
    glBindTexture(GL_TEXTURE_2D, uiTexture);
    glBegin(GL_QUADS);
    drawTexQuad(0, 240 - 12, 240, 12, UIBH, 0, 0, PTC(239), PTC(11));
    drawTexQuad(0, 0, 240, 116, UIBH, 0, PTC(26), PTC(239), PTC(141));
    glEnd();
    glDrawTextCentered("Equip ship", 2, TEXT_DKGREY);

    char buffer[32];
    char name[21];
    char status[5];

    glDrawText("ITEM               PRICE  QTY", 4, 128, TEXT_GREEN);
    for(uint8_t i = 0; i < NUM_EQUIPMENT_TYPES; i++)
    {
        printNameForEquipment(name, i);
        printEquipmentStatusForShip(status, player, i);
        sprintf(buffer, "%-19s %4d %4s", name, getPriceForEquipment(i), status);

        if(i == cursor)
        {
            glDrawText(buffer, 4, 136 + i * 8, 0x00FFFF);
        }
        else
        {
            glDrawText(buffer, 4, 136 + i * 8, TEXT_WHITE);
        }
    }
    
    sprintf(buffer, "%d credits", player->hold.money);
    glDrawTextCentered(buffer, 218, TEXT_DKGREY);

    glDrawText("Trading", 12, 240 - 10, TEXT_DKGREY);
    glDrawText("Contracts", 240 - 9 * 8 - 12, 240 - 10, TEXT_DKGREY);
}

void moveEquipUICursor(int8_t dir)
{
    cursor += dir;
    if(cursor < 0)
    {
        cursor = NUM_EQUIPMENT_TYPES - 1;
    }
    else if(cursor >= NUM_EQUIPMENT_TYPES)
    {
        cursor = 0;
    }
}

int8_t getEquipUICursor()
{
    return cursor;
}