#include "equipment.h"

uint16_t equipmentPrices[] = {
    [Fuel] = 2,
    [Cargo30] = 1500,
    [LaserMkII] = 2000,
    [LaserMkIII] = 4000,
    [MiningLaser] = 2000,
    [DockingComputer] = 450,
    [FuelScoops] = 800
};

const char* equipmentNames[] = {
    [Fuel] = "Fuel (0.5)",
    [Cargo30] = "Cargo hold (30)",
    [LaserMkII] = "Laser MkII",
    [LaserMkIII] = "Laser MkIII",
    [MiningLaser] = "Mining laser",
    [DockingComputer] = "Docking computer",
    [FuelScoops] = "Fuel scoops"
};

uint16_t getPriceForEquipment(EquipmentType type)
{
    return equipmentPrices[type];
}

void printNameForEquipment(char* str, EquipmentType type)
{
    strcpy(str, equipmentNames[type]);
}

void printOwn(char* str, bool own)
{
    if(own)
    {
        strcpy(str, "OWN");
    }
    else
    {
        strcpy(str, " / ");
    }
}

void printEquipmentStatusForShip(char* str, Player* player, EquipmentType type)
{
    switch(type)
    {
        case Fuel:
        {
            sprintf(str, "%2.1f", player->fuel);
            break;
        }
        case Cargo30:
        {
            printOwn(str, player->hold.size >= 30);
            break;
        }
        case LaserMkII:
        {
            printOwn(str, player->ship.weapon.type == 1);
            break;
        }
        case LaserMkIII:
        {
            printOwn(str, player->ship.weapon.type == 2);
            break;
        }
        case MiningLaser:
        {
            printOwn(str, player->ship.weapon.type == 3);
            break;
        }
        case DockingComputer:
        {
            printOwn(str, player->hasAutodock);
            break;
        }
        case FuelScoops:
        {
            printOwn(str, player->hasFuelScoops);
            break;
        }
    }
}

bool buyEquipment(Player* player, EquipmentType type)
{
    uint16_t price = equipmentPrices[type];
    if(player->hold.money < price)
    {
        return false;
    }

    //Sell old equipment (if applicable)
    uint16_t sellPrice = 0;
    switch(type)
    {
        case LaserMkII:
        case LaserMkIII:
        case MiningLaser:
        {
            switch(player->ship.weapon.type)
            {
                //Laser Mk II
                case 1:
                {
                    sellPrice = equipmentPrices[LaserMkII];
                    break;
                }
                //Laser Mk III
                case 2:
                {
                    sellPrice = equipmentPrices[LaserMkIII];
                    break;
                }
                //Mining Laser
                case 3:
                {
                    sellPrice = equipmentPrices[MiningLaser];
                    break;
                }
            }
            break;
        }
    }
    player->hold.money += sellPrice;
    
    //Buy new equipment
    switch(type)
    {
        case Fuel:
        {
            if(player->fuel < MAX_FUEL)
            {
                player->fuel += 1;
                if(player->fuel > MAX_FUEL)
                {
                    player->fuel = MAX_FUEL;
                }
                player->hold.money -= price;
            }
            break;
        }
        case Cargo30:
        {
            if(player->hold.size != 30)
            {
                player->hold.size = 30;
                player->hold.money -= price;
            }
            break;
        }
        case LaserMkII:
        {
            player->ship.weapon.type = 1;
            player->hold.money -= price;
            break;
        }
        case LaserMkIII:
        {
            player->ship.weapon.type = 2;
            player->hold.money -= price;
            break;
        }
        case MiningLaser:
        {
            player->ship.weapon.type = 3;
            player->hold.money -= price;
            break;
        }
        case DockingComputer:
        {
            if(!player->hasAutodock)
            {
                player->hasAutodock = true;
                player->hold.money -= price;
            }
            break;
        }
        case FuelScoops:
        {
            if(!player->hasFuelScoops)
            {
                player->hasFuelScoops = true;
                player->hold.money -= price;
            }
            break;
        }
    }
    return false;
}