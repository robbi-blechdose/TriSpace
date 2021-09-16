#include "cargo.h"

uint8_t getPriceForCargo(CargoType type, SystemInfo* info)
{
    switch(type)
    {
        case Food:
        {
            return 5 + info->waterDiff + (info->techLevel / 4);
            break;
        }
        case Textiles:
        {
            return 7 + info->treeDiff + (info->techLevel / 4);
            break;
        }
        case Liquor:
        {
            return 10 - info->government * 2;
            break;
        }
        case Computers:
        {
            return 50 + (MAX_TECH_LEVEL - info->techLevel) * 5;
            break;
        }
        case Machinery:
        {
            return 55 + (MAX_TECH_LEVEL - info->techLevel) * 3;
            break;
        }
        case Slaves:
        {
            return 50 + ((float) info->government / MAX_GOVERNMENT) * 150.0f;
        }
        case Firearms:
        {
            return 20 + (MAX_TECH_LEVEL / (float) info->techLevel) * 40 + ((float) info->government / MAX_GOVERNMENT) * 40;
        }
        case Narcotics:
        {
            return 10 + (MAX_TECH_LEVEL / (float) info->techLevel) * 20 + ((float) info->government / MAX_GOVERNMENT) * 40;
        }
    }
}

void printNameForCargo(char* str, CargoType type)
{
    switch(type)
    {
        case Food:
        {
            strcpy(str, "Food        ");
            break;
        }
        case Textiles:
        {
            strcpy(str, "Textiles    ");
            break;
        }
        case Liquor:
        {
            strcpy(str, "Liquor      ");
            break;
        }
        case Computers:
        {
            strcpy(str, "Computers   ");
            break;
        }
        case Machinery:
        {
            strcpy(str, "Machinery   ");
            break;
        }
        case Slaves:
        {
            strcpy(str, "Slaves      ");
            break;
        }
        case Firearms:
        {
            strcpy(str, "Firearms    ");
            break;
        }
        case Narcotics:
        {
            strcpy(str, "Narcotics   ");
            break;
        }
    }
}

void printUnitForCargo(char* str, CargoType type)
{
    switch(type)
    {
        case Food:
        case Textiles:
        case Computers:
        case Machinery:
        case Firearms:
        case Narcotics:
        case Slaves:
        {
            strcpy(str, "  t");
            break;
        }
        case Liquor:
        {
            strcpy(str, "  l");
            break;
        }
    }
}

uint8_t getCargoHoldSize(CargoHold* hold)
{
    uint8_t size = 0;
    for(uint8_t i = 0; i < NUM_CARGO_TYPES; i++)
    {
        size += hold->cargo[i];
    }
    return size;
}

uint8_t transferCargo(CargoHold* holdSell, CargoHold* holdBuy, CargoType type, SystemInfo* info)
{
    uint8_t cost = getPriceForCargo(type, info);
    if(holdBuy->money >= cost && getCargoHoldSize(holdBuy) < holdBuy->size && holdSell->cargo[type] > 0)
    {
        holdBuy->money -= cost;
        holdSell->money += cost;
        holdSell->cargo[type]--;
        holdBuy->cargo[type]++;
        return 1;
    }
    return 0;
}

void createStationHold(CargoHold* hold)
{
    hold->money = 50000;
    hold->size = 65535;
    for(uint8_t i = 0; i < NUM_CARGO_TYPES; i++)
    {
        hold->cargo[i] = 99;
    }
}