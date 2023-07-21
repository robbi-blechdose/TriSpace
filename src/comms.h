#ifndef _COMMS_H
#define _COMMS_H

#include <stdint.h>

void calcComms(uint32_t ticks);
void drawComms();

typedef enum {
    SENDER_ENEMY,
    SENDER_POLICE,
    SENDER_ALIEN,
    NUM_SENDER_TYPES
} CommSender;

typedef enum {
    TYPE_INTRO,
    TYPE_DAMAGE,
    TYPE_SPECIAL
} CommType;

void setCommMessage(CommSender sender, CommType type);

#define STATION_LAND_CLEAR 0

void setStationCommMessage(uint8_t index);

#endif