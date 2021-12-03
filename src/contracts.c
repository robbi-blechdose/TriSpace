#include "contracts.h"
#include "engine/util.h"
#include "universe/universe.h"

const char* contractTypes[NUM_CONTRACT_TYPES] = {
    "Obtain cargo",
    "Smuggle cargo",
    "Destroy ship"
};

const char* contractFirstnames[NUM_FIRSTNAMES] = {
    "Dennis",
    "Kevin",
    "Lee",
    "Dave",
    "Jason",
    "Robert",
    "John",

    "Kate",
    "Jill",
    "Susan",
    "Heather",
    "Danielle",
    "Riley"
};

const char* contractLastnames[NUM_LASTNAMES] = {
    "Thatcher",
    "Fletcher",
    "Wagner",
    "Hooper",
    "McCree",
    "Smith",
    "Davies",
    "Brown",
    "Hathaway",
    "Shelby",
    "Fox"
};

void selectTargetSystem(Contract* c, uint8_t currentStarSystem[2], uint8_t contractDifficulty)
{
    uint8_t possibleSystems[24][2]; //Contract difficulty must be between 1 - 3
    uint8_t possibleSystemsIndex = 0;

    float maxDistance = 7.0f * contractDifficulty;

    for(uint8_t i = 0; i < 16; i++)
    {
        for(uint8_t j = 0; j < 16; j++)
        {
            uint8_t targetSystem[2] = {i, j};
            if(getDistanceToSystem(currentStarSystem, targetSystem) <= maxDistance)
            {
                possibleSystems[possibleSystemsIndex][0] = i;
                possibleSystems[possibleSystemsIndex++][1] = 0;
            }
        }
    }

    uint8_t targetIndex = randr(possibleSystemsIndex - 1);
    c->targetSystem[0] = possibleSystems[targetIndex][0];
    c->targetSystem[1] = possibleSystems[targetIndex][1];
}

Contract generateContract(uint8_t currentStarSystem[2], SystemInfo* info)
{
    Contract c;
    c.type = randr(NUM_CONTRACT_TYPES);
    c.employerFirstname = randr(NUM_FIRSTNAMES - 1);
    c.employerLastname = randr(NUM_LASTNAMES - 1);

    switch(c.type)
    {
        case CONTRACT_GET_ITEM:
        {
            selectTargetSystem(&c, currentStarSystem, 1);
            c.cargo = randr(NUM_CARGO_TYPES);
            c.cargoAmount = 1 + randr(20);
            c.pay = getPriceForCargo(c.cargo, info) * c.cargoAmount + 50 + randr(400);
            break;
        }
        case CONTRACT_SMUGGLE:
        {
            if(randr(100) > 40)
            {
                //Select illegal cargo type
                c.cargo = randr(100) > 50 ? Narcotics : Slaves;
                selectTargetSystem(&c, currentStarSystem, 2);
            }
            else
            {
                c.cargo = randr(NUM_CARGO_TYPES -1);
                selectTargetSystem(&c, currentStarSystem, 1);
            }
            c.targetPosition = (vec3){0, 0, 0}; //TODO: Get position of space station in target system!
            c.cargoAmount = 5 + randr(5) * 4;
            //Base pay + cargo price / 2 + pay on top + pay for illegal cargo
            c.pay = 200 + (getPriceForCargo(c.cargo, info) * c.cargoAmount) / 2 + randr(200) * 10
                        + isCargoIllegal(c.cargo) * 500;
            //TODO
            break;
        }
        case CONTRACT_DESTROY_SHIP:
        {
            selectTargetSystem(&c, currentStarSystem, 1);
            //TODO
            c.targetPosition.x = 0;
            c.targetPosition.y = 0;
            c.targetPosition.z = 0;
        }
    }
    return c;
}

void generateContractsForSystem(Contract stationContracts[], uint8_t* numStationContracts, SystemInfo* info, uint8_t currentSystem[2])
{
    *numStationContracts = MIN_STATION_CONTRACTS + randr(MAX_STATION_CONTRACTS - MIN_STATION_CONTRACTS);

    for(uint8_t i = 0; i < *numStationContracts; i++)
    {
        stationContracts[i] = generateContract(currentSystem, info);
    }
}

uint8_t activateContract(Contract* contract, CargoHold* playerHold)
{
    switch(contract->type)
    {
        case CONTRACT_GET_ITEM:
        {
            return 1;
        }
        case CONTRACT_SMUGGLE:
        {
            //Check if there's space for the cargo
            if(playerHold->size >= (getCargoHoldSize(playerHold) + contract->cargoAmount))
            {
                playerHold->cargo[contract->cargo] += contract->cargoAmount;
                return 1;
            }
            break;
        }
        case CONTRACT_DESTROY_SHIP:
        {
            return 1;
        }
    }
    return 0;
}

uint8_t checkContract(Contract* contract, CargoHold* playerHold, uint8_t currentSystem[2])
{
    if(currentSystem[0] != contract->targetSystem[0] || currentSystem[1] != contract->targetSystem[1])
    {
        return 0;
    }

    switch(contract->type)
    {
        case CONTRACT_GET_ITEM:
        case CONTRACT_SMUGGLE:
        {
            if(currentSystem == contract->targetSystem)
            {
                if(playerHold->cargo[contract->cargo] >= contract->cargoAmount)
                {
                    playerHold->cargo[contract->cargo] -= contract->cargoAmount;
                    playerHold->money += contract->pay;
                    return 1;
                }
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

void contractStarSystemSetup(Contract* contract, Ship npcShips[], uint8_t currentSystem[2])
{
    if(contract->type == CONTRACT_TYPE_NULL)
    {
        return;
    }

    if(currentSystem[0] == contract->targetSystem[0] && currentSystem[1] == contract->targetSystem[1])
    {
        switch(contract->type)
        {
            case CONTRACT_DESTROY_SHIP:
            {
                //TODO: Somehow track if the ship is destroyed
                for(uint8_t i = 0; i < MAX_NPC_SHIPS; i++)
                {
                    if(npcShips[i].type == SHIP_TYPE_NULL)
                    {
                        //Found a spot, create ship
                        npcShips[i].type = SHIP_TYPE_CRUISELINER;
                        npcShips[i].position.x = randf(500) - 250;
                        npcShips[i].position.z = randf(500) - 250;
                        npcShips[i].position.y = randf(50) - 25;
                        break;
                    }
                }
                break;
            }
        }
    }
}

void printObjective(char* str, Contract* contract)
{
    char name[15];
    char unit[4];
    printNameForCargo(name, contract->cargo);
    printUnitForCargo(unit, contract->cargo);
    
    switch(contract->type)
    {
        case CONTRACT_GET_ITEM:
        {
            sprintf(str, "Deliver %d%s %s.", contract->cargoAmount, unit, name);
            break;
        }
        case CONTRACT_SMUGGLE:
        {
            sprintf(str, "Smuggle %d%s %s.\nWatch out for police\nships!", contract->cargoAmount, unit, name);
            break;
        }
        case CONTRACT_DESTROY_SHIP:
        {
            sprintf(str, "Destroy the cruise liner.");
            break;
        }
    }
}