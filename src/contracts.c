#include "contracts.h"
#include "engine/util.h"
#include "universe/satellites.h"

const char* contractTypes[NUM_CONTRACT_TYPES] = {
    "Obtain cargo",
    "Smuggle cargo",
    "Destroy ship",
    "Reconnaissance"
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
            //Select illegal cargo type
            c.cargo = Slaves + randr(Narcotics - Slaves);
            difficulty = 2;
            c.cargoAmount = 5 + randr(5) * 4;
            //Base pay + cargo price / 2 + pay on top
            c.pay = 400 + (getPriceForCargo(c.cargo, info) * c.cargoAmount) / 2 + randr(20) * 10;
            break;
        }
        case CONTRACT_DESTROY_SHIP:
        {
            c.pay = 300 + randr(80) * 5;
            break;
        }
        case CONTRACT_RECONNAISSANCE:
        {
            c.numSatellites = 3 + randr(NUM_SATELLITES - 3);
            c.pay = 200 + c.numSatellites * 100 + randr(25) * 8;
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
        case CONTRACT_RECONNAISSANCE:
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
        case CONTRACT_RECONNAISSANCE:
        {
            if(checkAllSatellitesVisited())
            {
                clearSatellites();
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
            //Spawn the cruise liner
            npcShips[NPC_SHIP_CONTRACT].type = SHIP_TYPE_CRUISELINER;
            vec3 pos = getRandomFreePos(starSystem, 20);
            npcShips[NPC_SHIP_CONTRACT].position.x = pos.x;
            npcShips[NPC_SHIP_CONTRACT].position.z = pos.z;
            npcShips[NPC_SHIP_CONTRACT].position.y = pos.y;

            //Spawn a few police ships to protect it
            uint8_t numPoliceShips = randr(5);
            for(uint8_t i = 0; i < numPoliceShips; i++)
            {
                vec3 policePos = getRandomFreePosBounds(starSystem, pos, (vec3) {.x = 100, .y = 50, .z = 100}, 25, 25);
                npcShips[i].type = SHIP_TYPE_POLICE;
                npcShips[i].position.x = policePos.x;
                npcShips[i].position.y = policePos.y;
                npcShips[i].position.z = policePos.z;
            }
            break;
        }
        case CONTRACT_RECONNAISSANCE:
        {
            vec3 pos = getRandomFreePosBounds(starSystem, starSystem->station.position, (vec3) {.x = 75, .y = 25, .z = 75}, 25, 50);
            createSatellite(pos, 0);
            for(uint8_t i = 1; i < contract->numSatellites; i++)
            {
                //TODO: improve
                pos = getRandomFreePosBounds(starSystem, pos, (vec3) {.x = 75, .y = 50, .z = 75}, 25, 50);
                createSatellite(pos, randf(2 * M_PI));
            }
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
        case CONTRACT_RECONNAISSANCE:
        {
            sprintf(str, "Check out each of the %d\nnav satellites.\nKeep your eyes open!", contract->numSatellites);
            break;
        }
    }
}