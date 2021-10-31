#ifndef _CONTRACTS_H
#define _CONTRACTS_H

#include <stdint.h>
#include "engine/includes/3dMath.h"
#include "cargo.h"

#define NUM_CONTRACT_TYPES 3
#define CONTRACT_GET_ITEM       0
#define CONTRACT_SMUGGLE        1
#define CONTRACT_DESTROY_SHIP   2
#define CONTRACT_TYPE_NULL    255
extern const char* contractTypes[NUM_CONTRACT_TYPES];

typedef struct {
    uint8_t type;

    uint16_t pay;

    uint32_t targetSystem;
    vec3 targetPosition;
    CargoType cargo;
    uint8_t cargoAmount;

    uint8_t employerFirstname;
    uint8_t employerLastname;
} Contract;

#define NUM_FIRSTNAMES 10
extern const char* contractFirstnames[NUM_FIRSTNAMES];
#define NUM_LASTNAMES 8
extern const char* contractLastnames[NUM_LASTNAMES];

Contract generateContract(uint32_t currentStarSystem, SystemInfo* info);
uint8_t checkContract(Contract* contract, CargoHold* playerHold, uint32_t currentSystem);
void printObjective(char* str, Contract* contract);

#endif