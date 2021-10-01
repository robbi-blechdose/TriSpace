#include "savegame.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE* saveFile;

void openSave(char* name, uint8_t writing)
{
    char buffer[255];
    strcpy(buffer, getenv("HOME"));
    strcat(buffer, name);
    if(writing)
    {
        saveFile = fopen(buffer, "w+b");
    }
    else
    {
        saveFile = fopen(buffer, "rb");
    }
}

void writeElement(void* ptr, uint16_t size)
{
    fwrite(ptr, size, 1, saveFile);
}

void readElement(void* ptr, uint16_t size)
{
    fread(ptr, size, 1, saveFile);
}

void closeSave()
{
    if(saveFile != NULL)
    {
        fclose(saveFile);
    }
}