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

const char* cargoNames[] = {
    [Food] = "Food",
    [Textiles] = "Textiles",
    [Liquor] = "Liquor",
    [Furs] = "Furs",
    [Radioactives] = "Radioactives",
    [Luxuries] = "Luxuries",
    [Computers] = "Computers",
    [Machinery] = "Machinery",
    [Gold] = "Gold",
    [Platinum] = "Platinum",
    [Dilithium] = "Dilithium",
    [Slaves] = "Slaves",
    [Firearms] = "Firearms",
    [Narcotics] = "Narcotics"
};

void printNameForCargo(char* str, CargoType type)
{
    strcpy(str, cargoNames[type]);
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

bool isCargoIllegal(CargoType type)
{
    switch(type)
    {
        case Slaves:
        case Firearms:
        case Narcotics:
        {
            return true;
        }
        default:
        {
            return false;
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

bool transferCargo(CargoHold* holdSell, CargoHold* holdBuy, CargoType type, SystemInfo* info, uint8_t limit)
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
        return true;
    }
    return false;
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