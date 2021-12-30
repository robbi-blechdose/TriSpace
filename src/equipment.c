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
    }
}

uint8_t buyEquipment(Ship* ship, EquipmentType type)
{
    uint16_t price = getPriceForEquipment(type);
    if(ship->hold.money < price)
    {
        return 0;
    }
    ship->hold.money-= price;

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
            }
            break;
        }
        case Cargo30:
        {
            ship->hold.size = 30;
            break;
        }
        case LaserMkII:
        {
            ship->weapon.type = 1;
            break;
        }
    }
    return 0;
}