#include "equipment.h"

uint16_t getPriceForEquipment(EquipmentType type)
{
    switch(type)
    {
        case Fuel:
        {
            return 2;
        }
        case Cargo30:
        {
            return 1500;
        }
        case LaserMkII:
        {
            return 2000;
        }
        case LaserMkIII:
        {
            return 4000;
        }
        case MiningLaser:
        {
            return 2000;
        }
        case DockingComputer:
        {
            return 450;
        }
    }
    return 0;
}

void printNameForEquipment(char* str, EquipmentType type)
{
    switch(type)
    {
        case Fuel:
        {
            strcpy(str, "Fuel (0.5)");
            break;
        }
        case Cargo30:
        {
            strcpy(str, "Cargo hold (30)");
            break;
        }
        case LaserMkII:
        {
            strcpy(str, "Laser MkII");
            break;
        }
        case LaserMkIII:
        {
            strcpy(str, "Laser MkIII");
            break;
        }
        case MiningLaser:
        {
            strcpy(str, "Mining laser");
            break;
        }
        case DockingComputer:
        {
            strcpy(str, "Docking computer");
            break;
        }
    }
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