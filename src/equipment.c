#include "equipment.h"

uint16_t equipmentPrices[] = {
    [Fuel] = 2,
    [Cargo30] = 1500,
    [LaserMkII] = 2000,
    [LaserMkIII] = 4000,
    [MiningLaser] = 2000,
    [DockingComputer] = 450
};

const char* equipmentNames[] = {
    [Fuel] = "Fuel (0.5)",
    [Cargo30] = "Cargo hold (30)",
    [LaserMkII] = "Laser MkII",
    [LaserMkIII] = "Laser MkIII",
    [MiningLaser] = "Mining laser",
    [DockingComputer] = "Docking computer"
};

uint16_t getPriceForEquipment(EquipmentType type)
{
    return equipmentPrices[type];
}

void printNameForEquipment(char* str, EquipmentType type)
{
    strcpy(str, equipmentNames[type]);
}

void printOwn(char* str, uint8_t own)
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

void printEquipmentStatusForShip(char* str, Ship* ship, EquipmentType type)
{
    switch(type)
    {
        case Fuel:
        {
            sprintf(str, "%1.1f", ship->fuel / 10.0f);
            break;
        }
        case Cargo30:
        {
            printOwn(str, ship->hold.size >= 30);
            break;
        }
        case LaserMkII:
        {
            printOwn(str, ship->weapon.type == 1);
            break;
        }
        case LaserMkIII:
        {
            printOwn(str, ship->weapon.type == 2);
            break;
        }
        case MiningLaser:
        {
            printOwn(str, ship->weapon.type == 3);
            break;
        }
        case DockingComputer:
        {
            printOwn(str, ship->hasAutodock);
            break;
        }
    }
}

uint8_t buyEquipment(Ship* ship, EquipmentType type)
{
    uint16_t price = getPriceForEquipment(type);
    if(ship->hold.money < price)
    {
        return 0;
    }

    //Sell old equipment (if applicable)
    uint16_t sellPrice = 0;
    switch(type)
    {
        case LaserMkII:
        case LaserMkIII:
        case MiningLaser:
        {
            switch(ship->weapon.type)
            {
                //Laser Mk II
                case 1:
                {
                    sellPrice = getPriceForEquipment(LaserMkII);
                    break;
                }
                //Laser Mk III
                case 2:
                {
                    sellPrice = getPriceForEquipment(LaserMkIII);
                    break;
                }
                //Mining Laser
                case 3:
                {
                    sellPrice = getPriceForEquipment(MiningLaser);
                    break;
                }
            }
            break;
        }
    }
    ship->hold.money += sellPrice;
    
    //Buy new equipment
    switch(type)
    {
        case Fuel:
        {
            if(ship->fuel < MAX_FUEL)
            {
                ship->fuel += 5;
                if(ship->fuel > MAX_FUEL)
                {
                    ship->fuel = MAX_FUEL;
                }
                ship->hold.money -= price;
            }
            break;
        }
        case Cargo30:
        {
            if(ship->hold.size != 30)
            {
                ship->hold.size = 30;
                ship->hold.money -= price;
            }
            break;
        }
        case LaserMkII:
        {
            ship->weapon.type = 1;
            ship->hold.money -= price;
            break;
        }
        case LaserMkIII:
        {
            ship->weapon.type = 2;
            ship->hold.money -= price;
            break;
        }
        case MiningLaser:
        {
            ship->weapon.type = 3;
            ship->hold.money -= price;
            break;
        }
        case DockingComputer:
        {
            if(!ship->hasAutodock)
            {
                ship->hasAutodock = 1;
                ship->hold.money -= price;
            }
            break;
        }
    }
    return 0;
}