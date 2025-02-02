#include "comms.h"

#include <string.h>
#include <stdio.h>
#include "GL/gl.h"
#include "fk-engine-core/util.h"
#include "fk-engine-core/text.h"

#include "ui/uiutils.h"

typedef struct {
    uint8_t length;
    const char** list;
} CommList;

const CommList introComms[NUM_SENDER_TYPES] = {
    [SENDER_ENEMY]  = {3, (const char*[3]) {"Prepare to die!", "Accept your fate!", "That ship is scrap."}},
    [SENDER_POLICE] = {4, (const char*[4]) {"Prepare to be boarded.", "Shut down your engines.", "Surrender your ship.", "You're breaking the law!"}},
    [SENDER_ALIEN]  = {2, (const char*[2]) {"%!& #&%[$   [$!%+#", "#+*$!% §$%&'+#"}}
};

const CommList damageComms[NUM_SENDER_TYPES] = {
    [SENDER_ENEMY]  = {3, (const char*[3]) {"You'll pay for that!", "Ow!", "Arrr!"}},
    [SENDER_POLICE] = {1, (const char*[2]) {"Cease your attacks!"}},
    [SENDER_ALIEN]  = {2, (const char*[2]) {"$%&%!", "#*!$"}}
};

const CommList specialComms[NUM_SENDER_TYPES] = {
    [SENDER_ENEMY]  = {},
    [SENDER_POLICE] = {2, (const char*[2]) {"Illegal goods detected!", "You're carrying illegal goods."}},
    [SENDER_ALIEN]  = {}
};

#define NUM_COMM_FLASHES 4
const uint32_t commFlashes[5][2] = {
    {0, 75},
    {125, 200},
    {250, 325},
    {400, 1700}
};

const char* commHeaderIncoming = "INCOMING TRANSMISSION";
const char* commHeaderInternal = "SYSTEM:";

uint32_t commTicks;
uint8_t commFlashIndex;
const char* commHeader;
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
    glDrawTextCentered(commHeader, 16, 0xFFFFFF);
    glDrawTextCentered(commMessage, 140, 0xFFFFFF);
}

void setCommMessage(CommSender sender, CommType type)
{
    commTicks = 1;
    commFlashIndex = 0;

    commHeader = commHeaderIncoming;
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

    commHeader = commHeaderIncoming;

    switch(index)
    {
        case STATION_LAND_CLEAR:
        {
            sprintf(buffer, "Cleared to land in bay %d.", randr(5) + 1);
            commMessage = buffer;
            break;
        }
    }
}

const char* systemComms[NUM_SYSTEM_COMMS] = {
    [SC_AUTODOCK_ENABLED] = "Autodocking enabled.",
    [SC_FUEL_SCOOPS_DONE] = "Fuel tanks full.",
    [SC_MISSILES_EMPTY] = "Missiles depleted."
};

void setSystemCommMessage(SystemComm comm)
{
    commTicks = 1;
    commFlashIndex = 0;

    commHeader = commHeaderInternal;
    commMessage = systemComms[comm];
}