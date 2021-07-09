#ifndef _UTIL_H
#define _UTIL_H

#include <math.h>

typedef struct {
    float x;
    float y;
    float z;
} Vector3;

#define RAD_TO_DEG(rad) ((rad) * 180.0f / M_PI)

#endif