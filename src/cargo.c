#include "cargo.h"

uint8_t getPriceForCargo(CargoType type, SystemInfo* info)
{
    switch(type)
    {
        case Food:
        {
            return 5 + info->waterDiff + (info->techLevel / 4);
        }
        case Textiles:
        {
            return 7 + info->treeDiff + (info->techLevel / 4);
        }
        case Liquor:
        {
            return 10 - info->government * 2;
        }
        case Furs:
        {
            return 7 + ((float) info->techLevel / MAX_TECH_LEVEL) * 5;
        }
        case Radioactives:
        {
            return 20 + ((float) info->techLevel / MAX_TECH_LEVEL) * 10;
        }
        case Luxuries:
        {
            return 60 + (((float) (MAX_TECH_LEVEL - info->techLevel)) / MAX_TECH_LEVEL) * 60;
        }
        case Computers:
        {
            return 50 + (MAX_TECH_LEVEL - info->techLevel) * 5;
        }
        case Machinery:
        {
            return 55 + (MAX_TECH_LEVEL - info->techLevel) * 3;
        }
        case Gold:
        {
            return 45 + (((float) (MAX_TECH_LEVEL - info->techLevel)) / MAX_TECH_LEVEL) * 30;
        }
        case Platinum:
        {
            return 55 + (((float) (MAX_TECH_LEVEL - info->techLevel)) / MAX_TECH_LEVEL) * 30;
        }
        case Dilithium:
        {
            return 10 + ((float) info->techLevel / MAX_TECH_LEVEL) * 90;
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
            strcpy(str, "Food");
            break;
        }
        case Textiles:
        {
            strcpy(str, "Textiles");
            break;
        }
        case Liquor:
        {
            strcpy(str, "Liquor");
            break;
        }
        case Furs:
        {
            strcpy(str, "Furs");
            break;
        }
        case Radioactives:
        {
            strcpy(str, "Radioactives");
            break;
        }
        case Luxuries:
        {
            strcpy(str, "Luxuries");
            break;
        }
        case Computers:
        {
            strcpy(str, "Computers");
            break;
        }
        case Machinery:
        {
            strcpy(str, "Machinery");
            break;
        }
        case Gold:
        {
            strcpy(str, "Gold");
            break;
        }
        case Platinum:
        {
            strcpy(str, "Platinum");
            break;
        }
        case Dilithium:
        {
            strcpy(str, "Dilithium");
            break;
        }
        case Slaves:
        {
            strcpy(str, "Slaves");
            break;
        }
        case Firearms:
        {
            strcpy(str, "Firearms");
            break;
        }
        case Narcotics:
        {
            strcpy(str, "Narcotics");
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
        case Furs:
        case Radioactives:
        case Luxuries:
        case Computers:
        case Machinery:
        case Firearms:
        case Narcotics:
        case Slaves:
        {
            strcpy(str, "t");
            break;
        }
        case Liquor:
        {
            strcpy(str, "l");
            break;
        }
        case Gold:
        case Platinum:
        case Dilithium:
        {
            strcpy(str, "kg");
        }
    }
}

uint8_t isCargoIllegal(CargoType type)
{
    switch(type)
    {
        case Slaves:
        case Firearms:
        case Narcotics:
        {
            return 1;
        }
    }
    return 0;
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

uint8_t transferCargo(CargoHold* holdSell, CargoHold* holdBuy, CargoType type, SystemInfo* info, uint8_t limit)
{
    uint8_t cost = getPriceForCargo(type, info);
    if(holdBuy->money >= cost && getCargoHoldSize(holdBuy) < holdBuy->size && holdSell->cargo[type] > 0)
    {
        holdBuy->money -= cost;
        holdSell->money += cost;
        holdSell->cargo[type]--;
        holdBuy->cargo[type]++;
        if(limit)
        {
            if(holdBuy->cargo[type] > 99)
            {
                holdBuy->cargo[type] = 99;
            }
        }
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