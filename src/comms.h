#ifndef _COMMS_H
#define _COMMS_H

#include <stdint.h>

void calcComms(uint32_t ticks);
void drawComms();

#define SENDER_ENEMY 0
#define SENDER_POLICE 1
#define SENDER_ALIEN 2

#define TYPE_INTRO 0
#define TYPE_DAMAGE 1
#define TYPE_SPECIAL 2

void setCommMessage(uint8_t sender, uint8_t type);

#endif