#ifndef _CONTRACTS_H
#define _CONTRACTS_H

#include <stdint.h>
#include <stdbool.h>

#include "engine/includes/3dMath.h"
#include "cargo.h"
#include "ship.h"
#include "universe/universe.h"
#include "npcs/npc.h"

#define NUM_CONTRACT_TYPES      4
#define CONTRACT_GET_ITEM       0
#define CONTRACT_SMUGGLE        1
#define CONTRACT_DESTROY_SHIP   2
#define CONTRACT_RECONNAISSANCE 3
#define CONTRACT_TYPE_NULL    255
extern const char* contractTypes[NUM_CONTRACT_TYPES];

typedef struct {
    uint8_t type;

    uint16_t pay;

    uint8_t employerFirstname;
    uint8_t employerLastname;

    int8_t targetSystem[2];

    CargoType cargo;
    union {
    uint8_t cargoAmount;
    uint8_t numSatellites;
    };
} Contract;

#define NUM_FIRSTNAMES 16
extern const char* contractFirstnames[NUM_FIRSTNAMES];
#define NUM_LASTNAMES 14
extern const char* contractLastnames[NUM_LASTNAMES];

#define MIN_STATION_CONTRACTS 2
#define MAX_STATION_CONTRACTS 5

Contract generateContract(int8_t currentStarSystem[2], SystemInfo* info, uint8_t contractIndex);
void generateContractsForSystem(Contract stationContracts[], uint8_t* numStationContracts, SystemInfo* info, int8_t currentSystem[2],
                                    uint8_t completedContracts[UNIVERSE_SIZE][UNIVERSE_SIZE]);

bool activateContract(Contract* contract, CargoHold* playerHold);
bool checkContract(Contract* contract, CargoHold* playerHold, int8_t currentSystem[2], Npc npcs[]);
void contractStarSystemSetup(Contract* contract, Npc npcs[], int8_t currentSystem[2], StarSystem* starSystem);
void printObjective(char* str, Contract* contract);

#endif