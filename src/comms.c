#include "comms.h"

#include <string.h>
#include <stdio.h>
#include "GL/gl.h"
#include "engine/util.h"

#include "ui/uiutils.h"

typedef struct {
    uint8_t length;
    char** list;
} CommList;

#define NUM_SENDER_TYPES 3
const CommList introComms[NUM_SENDER_TYPES] = {
    {3, (char*[3]) {"Prepare to die!", "Accept your fate!", "That ship is scrap."}},
    {4, (char*[4]) {"Prepare to be boarded.", "Shut down your engines.", "Surrender your ship.", "You're breaking the law!"}},
    {2, (char*[2]) {"%!& #&%[$   [$!%+#", "#+*$!% §$%&'+#"}}
};

const CommList damageComms[NUM_SENDER_TYPES] = {
    {3, (char*[3]) {"You'll pay for that!", "Ow!", "Arrr!"}},
    {1, (char*[2]) {"Cease your attacks!"}},
    {1, (char*[1]) {"$%&%!"}}
};

const CommList specialComms[NUM_SENDER_TYPES] = {
    {},
    {2, (char*[2]) {"Illegal goods detected!", "You're carrying illegal goods."}},
    {}
};

#define NUM_COMM_FLASHES 4
const uint32_t commFlashes[5][2] = {
    {0, 75},
    {125, 200},
    {250, 325},
    {400, 1700}
};

uint32_t commTicks;
uint8_t commFlashIndex;
const char* commMessage;

void calcComms(uint32_t ticks)
{
    if(commTicks)
    {
        commTicks += ticks;
    }
}

void drawComms()
{
    if(!commTicks)
    {
        return;
    }

    if(commTicks < commFlashes[commFlashIndex][0])
    {
        return;
    }

    if(commTicks > commFlashes[commFlashIndex][1])
    {
        commFlashIndex++;
        return;
    }

    if(commFlashIndex >= NUM_COMM_FLASHES)
    {
        commTicks = 0;
    }

    //We're within a flash, draw

    glDrawText("INCOMING TRANSMISSION", CENTER(21), 16, 0xFFFFFF);
    glDrawText(commMessage, CENTER(strlen(commMessage)), 140, 0xFFFFFF);
}

void setCommMessage(uint8_t sender, uint8_t type)
{
    commTicks = 1;
    commFlashIndex = 0;

    commMessage = introComms[sender].list[randr(introComms[sender].length - 1)];

    switch(type)
    {
        case TYPE_INTRO:
        {
            commMessage = introComms[sender].list[randr(introComms[sender].length - 1)];
            break;
        }
        case TYPE_DAMAGE:
        {
            commMessage = damageComms[sender].list[randr(damageComms[sender].length - 1)];
            break;
        }
        case TYPE_SPECIAL:
        {
            commMessage = specialComms[sender].list[randr(specialComms[sender].length - 1)];
            break;
        }
    }
}

static char buffer[30];

void setStationCommMessage(uint8_t index)
{
    commTicks = 1;
    commFlashIndex = 0;

    switch(index)
    {
        case STATION_LAND_CLEAR:
        {
            sprintf(buffer, "Cleared to land in bay %d.", randr(5) + 1);
            commMessage = buffer;
        }
    }
}