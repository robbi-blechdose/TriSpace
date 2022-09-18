#ifndef _EQUIP_UI_H
#define _EQUIP_UI_H

#include "../player.h"

void initEquipUI();

void calcEquipUI(uint32_t ticks);

void drawEquipUI3d(Player* player);
void drawEquipUI(Player* player);

void moveEquipUICursor(int8_t dir);
int8_t getEquipUICursor();

#endif