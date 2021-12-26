#include "contracts.h"
#include "engine/util.h"

const char* contractTypes[NUM_CONTRACT_TYPES] = {
    "Obtain cargo",
    "Smuggle cargo",
    "Destroy ship",
    "Escort ship"
};

const char* contractFirstnames[NUM_FIRSTNAMES] = {
    "Dennis",
    "Kevin",
    "Lee",
    "Dave",
    "Jason",
    "Robert",
    "John",
    "James",

    "Kate",
    "Jill",
    "Susan",
    "Heather",
    "Danielle",
    "Riley",
    "Jennifer",
    "Patricia"
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
    "Fox",
    "Hobson",
    "Rodriguez",
    "Moore",
    "Addams"
};

void selectTargetSystem(Contract* c, uint8_t currentStarSystem[2], uint8_t contractDifficulty)
{
    uint8_t possibleSystems[64][2];
    uint8_t possibleSystemsIndex = 0;

    float maxDistance = 7.0f * contractDifficulty;

    for(uint8_t i = 0; i < UNIVERSE_SIZE; i++)
    {
        for(uint8_t j = 0; j < UNIVERSE_SIZE; j++)
        {
            uint8_t targetSystem[2] = {i, j};
            if(getDistanceToSystem(currentStarSystem, targetSystem) <= maxDistance)
            {
                possibleSystems[possibleSystemsIndex][0] = i;
                possibleSystems[possibleSystemsIndex++][1] = j;
            }
        }
    }

    uint8_t targetIndex = randr(possibleSystemsIndex - 1);
    c->targetSystem[0] = possibleSystems[targetIndex][0];
    c->targetSystem[1] = possibleSystems[targetIndex][1];
}

Contract generateContract(uint8_t currentStarSystem[2], SystemInfo* info, uint8_t contractIndex)
{
    srand(currentStarSystem[0] + currentStarSystem[1] * UNIVERSE_SIZE + contractIndex * 255);
    Contract c;
    c.type = randr(NUM_CONTRACT_TYPES - 1);
    c.employerFirstname = randr(NUM_FIRSTNAMES - 1);
    c.employerLastname = randr(NUM_LASTNAMES - 1);
    uint8_t difficulty = 1;

    switch(c.type)
    {
        case CONTRACT_GET_ITEM:
        {
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
                difficulty = 2;
            }
            else
            {
                c.cargo = randr(NUM_CARGO_TYPES -1);
            }
            c.cargoAmount = 5 + randr(5) * 4;
            //Base pay + cargo price / 2 + pay on top + pay for illegal cargo
            c.pay = 200 + (getPriceForCargo(c.cargo, info) * c.cargoAmount) / 2 + randr(200) * 10
                        + isCargoIllegal(c.cargo) * 500;
            break;
        }
        case CONTRACT_DESTROY_SHIP:
        {
            c.pay = 300 + randr(80) * 5;
            break;
        }
        case CONTRACT_PROTECT_SHIP:
        {
            c.pay = 350 + randr(50) * 5;
            break;
        }
    }
    selectTargetSystem(&c, currentStarSystem, difficulty);
    return c;
}

void generateContractsForSystem(Contract stationContracts[], uint8_t* numStationContracts, SystemInfo* info, uint8_t currentSystem[2],
                                    uint8_t completedContracts[UNIVERSE_SIZE][UNIVERSE_SIZE])
{
    *numStationContracts = MIN_STATION_CONTRACTS + randr(MAX_STATION_CONTRACTS - MIN_STATION_CONTRACTS);

    for(uint8_t i = 0; i < *numStationContracts; i++)
    {
        stationContracts[i] = generateContract(currentSystem, info, completedContracts[currentSystem[0]][currentSystem[1]] + i);
    }
}

uint8_t activateContract(Contract* contract, CargoHold* playerHold)
{
    switch(contract->type)
    {
        case CONTRACT_GET_ITEM:
        case CONTRACT_DESTROY_SHIP:
        case CONTRACT_PROTECT_SHIP:
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
    }
    return 0;
}

uint8_t checkContract(Contract* contract, CargoHold* playerHold, uint8_t currentSystem[2], Ship npcShips[])
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
            if(npcShips[NPC_SHIP_CONTRACT].type == SHIP_TYPE_NULL)
            {
                playerHold->money += contract->pay;
                return 1;
            }
            break;
        }
        case CONTRACT_PROTECT_SHIP:
        {
            if(npcShips[NPC_SHIP_CONTRACT].type != SHIP_TYPE_NULL)
            {
                npcShips[NPC_SHIP_CONTRACT].type = SHIP_TYPE_NULL;
                playerHold->money += contract->pay;
                return 1;
            }
            break;
        }
    }
    return 0;
}

void contractStarSystemSetup(Contract* contract, Ship npcShips[], uint8_t currentSystem[2], StarSystem* starSystem)
{
    if(contract->type == CONTRACT_TYPE_NULL)
    {
        return;
    }
    if(currentSystem[0] != contract->targetSystem[0] || currentSystem[1] != contract->targetSystem[1])
    {
        return;
    }

    switch(contract->type)
    {
        case CONTRACT_DESTROY_SHIP:
        {
            npcShips[NPC_SHIP_CONTRACT].type = SHIP_TYPE_CRUISELINER;
            vec3 pos = getRandomFreePos(starSystem, 20);
            npcShips[NPC_SHIP_CONTRACT].position.x = pos.x;
            npcShips[NPC_SHIP_CONTRACT].position.z = pos.z;
            npcShips[NPC_SHIP_CONTRACT].position.y = pos.y;
            break;
        }
        case CONTRACT_PROTECT_SHIP:
        {
            //TODO
            vec3 pos = getRandomFreePos(starSystem, 20);
            npcShips[NPC_SHIP_CONTRACT].position.x = pos.x;
            npcShips[NPC_SHIP_CONTRACT].position.z = pos.z;
            npcShips[NPC_SHIP_CONTRACT].position.y = pos.y;
            break;
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
            sprintf(str, "Deliver %d%s of %s.", contract->cargoAmount, unit, name);
            break;
        }
        case CONTRACT_SMUGGLE:
        {
            sprintf(str, "Smuggle %d%s %s.\nWatch out for police\nships!", contract->cargoAmount, unit, name);
            break;
        }
        case CONTRACT_DESTROY_SHIP:
        {
            sprintf(str, "Destroy the cruise liner.\nYou might get some\npolice attention...");
            break;
        }
        case CONTRACT_PROTECT_SHIP:
        {
            sprintf(str, "Protect the cargo ship\non its way out of\nthe system.");
            break;
        }
    }
}