#include "contracts.h"

#include <stdio.h>
#include <stdlib.h>
#include "engine/util.h"
#include "universe/satellites.h"

const char* contractTypeNames[NUM_CONTRACT_TYPES] = {
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

void selectTargetSystem(Contract* c, int8_t currentStarSystem[2], uint8_t contractDifficulty)
{
    int8_t possibleSystems[64][2];
    uint8_t possibleSystemsIndex = 0;

    float maxDistance = MAX_FUEL * contractDifficulty;

    for(uint8_t i = 0; i < UNIVERSE_SIZE; i++)
    {
        for(uint8_t j = 0; j < UNIVERSE_SIZE; j++)
        {
            int8_t targetSystem[2] = {i, j};
            if(!(i == currentStarSystem[0] && j == currentStarSystem[1])
                && getDistanceToSystem(currentStarSystem, targetSystem) <= maxDistance)
            {
                possibleSystems[possibleSystemsIndex][0] = i;
                possibleSystems[possibleSystemsIndex++][1] = j;
            }
        }
    }

    if(possibleSystemsIndex == 0)
    {
        //No systems found. Shouldn't happen, but just in case. Fallback: current system
        c->targetSystem[0] = currentStarSystem[0];
        c->targetSystem[1] = currentStarSystem[1];
    }
    else
    {
        uint8_t targetIndex = randr(possibleSystemsIndex - 1);
        c->targetSystem[0] = possibleSystems[targetIndex][0];
        c->targetSystem[1] = possibleSystems[targetIndex][1];
    }
}

Contract generateContract(int8_t currentStarSystem[2], SystemCharacteristics* chars, uint8_t contractIndex)
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
            c.cargo = randr(NUM_CARGO_TYPES - 1);
            c.cargoAmount = 1 + randr(20);
            c.pay = getPriceForCargo(c.cargo, chars) * c.cargoAmount + 50 + randr(400);
            break;
        }
        case CONTRACT_SMUGGLE:
        {
            //Select illegal cargo type
            c.cargo = Slaves + randr(Narcotics - Slaves);
            difficulty = 2;
            c.cargoAmount = 5 + randr(5) * 4;
            //Base pay + cargo price / 2 + pay on top
            c.pay = 400 + (getPriceForCargo(c.cargo, chars) * c.cargoAmount) / 2 + randr(20) * 10;
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
            c.pay = 100 + c.numSatellites * 75 + randr(25) * 5;
            break;
        }
        default:
        {
            //This should not happen, mark with pay number
            c.pay = 54321;
            break;
        }
    }
    selectTargetSystem(&c, currentStarSystem, difficulty);
    return c;
}

void generateContractsForSystem(Contract stationContracts[], uint8_t* numStationContracts, SystemCharacteristics* chars, int8_t currentSystem[2],
                                    uint8_t completedContracts[UNIVERSE_SIZE][UNIVERSE_SIZE])
{
    srand((currentSystem[0] + currentSystem[1] * UNIVERSE_SIZE) * 10);

    *numStationContracts = MIN_STATION_CONTRACTS + randr(MAX_STATION_CONTRACTS - MIN_STATION_CONTRACTS);

    for(uint8_t i = 0; i < *numStationContracts; i++)
    {
        stationContracts[i] = generateContract(currentSystem, chars, completedContracts[currentSystem[0]][currentSystem[1]] + i);
    }
}

bool activateContract(Contract* contract, CargoHold* playerHold)
{
    switch(contract->type)
    {
        case CONTRACT_GET_ITEM:
        case CONTRACT_DESTROY_SHIP:
        case CONTRACT_RECONNAISSANCE:
        {
            return true;
        }
        case CONTRACT_SMUGGLE:
        {
            //Check if there's space for the cargo
            if(playerHold->size >= (getCargoHoldSize(playerHold) + contract->cargoAmount))
            {
                playerHold->cargo[contract->cargo] += contract->cargoAmount;
                return true;
            }
            break;
        }
        default:
        {
            //This should not happen
            return false;
        }
    }
    return false;
}

bool checkContract(Contract* contract, CargoHold* playerHold, int8_t currentSystem[2], Npc npcs[])
{
    if(currentSystem[0] != contract->targetSystem[0] || currentSystem[1] != contract->targetSystem[1])
    {
        return false;
    }

    switch(contract->type)
    {
        case CONTRACT_GET_ITEM:
        case CONTRACT_SMUGGLE:
        {
            if(currentSystem[0] == contract->targetSystem[0] && currentSystem[1] == contract->targetSystem[1])
            {
                if(playerHold->cargo[contract->cargo] >= contract->cargoAmount)
                {
                    playerHold->cargo[contract->cargo] -= contract->cargoAmount;
                    playerHold->money += contract->pay;
                    return true;
                }
            }
            break;
        }
        case CONTRACT_DESTROY_SHIP:
        {
            if(npcs[NPC_CONTRACT].ship.type == SHIP_TYPE_NULL)
            {
                playerHold->money += contract->pay;
                return true;
            }
            break;
        }
        case CONTRACT_RECONNAISSANCE:
        {
            if(checkAllSatellitesVisited())
            {
                clearSatellites();
                playerHold->money += contract->pay;
                return true;
            }
            break;
        }
        default:
        {
            //This should not happen
            return false;
        }
    }
    return false;
}

void contractStarSystemSetup(Contract* contract, Npc npcs[], int8_t currentSystem[2], StarSystem* starSystem)
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
            npcs[NPC_CONTRACT].ship.type = SHIP_TYPE_CRUISELINER;
            vec3 pos = getRandomFreePos(starSystem, 20);
            npcs[NPC_CONTRACT].ship.position.x = pos.x;
            npcs[NPC_CONTRACT].ship.position.z = pos.z;
            npcs[NPC_CONTRACT].ship.position.y = pos.y;

            //Spawn a few police ships to protect it
            uint8_t numPoliceShips = randr(5);
            for(uint8_t i = 0; i < numPoliceShips; i++)
            {
                vec3 policePos = getRandomFreePosBounds(starSystem, pos, (vec3) {.x = 100, .y = 50, .z = 100}, 25, 25);
                npcs[i].ship.type = SHIP_TYPE_POLICE;
                npcs[i].ship.position.x = policePos.x;
                npcs[i].ship.position.y = policePos.y;
                npcs[i].ship.position.z = policePos.z;
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
        default:
        {
            //Do nothing for all other types
            break;
        }
    }
}

void printObjective(char* str, Contract* contract)
{
    char name[15];
    char unit[4];

    switch(contract->type)
    {
        case CONTRACT_GET_ITEM:
        {
            printNameForCargo(name, contract->cargo);
            printUnitForCargo(unit, contract->cargo);
            sprintf(str, "Deliver %d%s of %s.", contract->cargoAmount, unit, name);
            break;
        }
        case CONTRACT_SMUGGLE:
        {
            printNameForCargo(name, contract->cargo);
            printUnitForCargo(unit, contract->cargo);
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
        default:
        {
            //This should not happen, print info into objective
            sprintf(str, "ERROR CONTRACT TYPE IS %d.", contract->type);
            break;
        }
    }
}