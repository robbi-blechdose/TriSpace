#ifndef CONTRACT_UI_H
#define CONTRACT_UI_H

#include "../contracts.h"

void initContractUI(GLuint uiTex);
void quitContractUI();

void calcContractUI(uint32_t ticks, Contract* activeContract, Contract* contracts);

void drawContractUI3d(Contract* activeContract, Contract* contracts);
void drawContractUI(Contract* activeContract, Contract* contracts, uint8_t numContracts);

void moveContractUICursor(int8_t dir, Contract* contracts, uint8_t numContracts);
int8_t getContractUICursor();
void setContractUICursor(int8_t value);

#endif