#include "contracts.h"
#include "engine/util.h"

const char* contractTypes[NUM_CONTRACT_TYPES] = {
    "Obtain cargo",
    "Destroy ship"
};

const char* contractFirstnames[NUM_FIRSTNAMES] = {
    "Dennis",
    "Kevin",
    "Lee",
    "Dave",
    "Jason",
    "Robert",
    "Kate",
    "Jill",
    "Susan",
    "Heather"
};

const char* contractLastnames[NUM_LASTNAMES] = {
    "Thatcher",
    "Fletcher",
    "Wagner",
    "Hooper",
    "McCree",
    "Smith",
    "Davies",
    "Brown"
};

Contract generateContract(uint32_t currentStarSystem, SystemInfo* info)
{
    Contract c;
    c.type = randr(NUM_CONTRACT_TYPES - 1);
    c.employerFirstname = randr(NUM_FIRSTNAMES - 1);
    c.employerLastname = randr(NUM_LASTNAMES - 1);

    switch(c.type)
    {
        case CONTRACT_GET_ITEM:
        {
            c.targetSystem = currentStarSystem;
            c.cargo = randr(NUM_CARGO_TYPES);
            c.cargoAmount = 1 + randr(20);
            c.pay = getPriceForCargo(c.cargo, info) * c.cargoAmount + 50 + randr(400);
            break;
        }
        case CONTRACT_DESTROY_SHIP:
        {
            c.targetSystem = currentStarSystem + randr(3); //TODO: Improve this once we have a proper system map going
            //TODO
            c.targetPosition.x = 0;
            c.targetPosition.y = 0;
            c.targetPosition.z = 0;
        }
    }
    return c;
}

uint8_t checkContract(Contract* contract, CargoHold* playerHold)
{
    switch(contract->type)
    {
        case CONTRACT_GET_ITEM:
        {
            if(playerHold->cargo[contract->type] >= contract->cargoAmount)
            {
                playerHold->cargo[contract->type] -= contract->cargoAmount;
                playerHold->money += contract->pay;
                return 1;
            }
            break;
        }
        case CONTRACT_DESTROY_SHIP:
        {
            break;
        }
    }
    return 0;
}

void printObjective(char* str, Contract* contract)
{
    switch(contract->type)
    {
        case CONTRACT_GET_ITEM:
        {
            char name[15];
            char unit[4];
            printNameForCargo(name, contract->cargo);
            printUnitForCargo(unit, contract->cargo);
            sprintf(str, "Deliver %2d %s %s.", contract->cargoAmount, unit, name);
            break;
        }
        case CONTRACT_DESTROY_SHIP:
        {
            break;
        }
    }
}