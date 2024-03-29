#ifndef COMMS_H
#define COMMS_H

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

typedef enum {
    SC_AUTODOCK_ENABLED,
    SC_FUEL_SCOOPS_DONE,
    SC_MISSILES_EMPTY,
    NUM_SYSTEM_COMMS
} SystemComm;

void setSystemCommMessage(SystemComm comm);

#endif