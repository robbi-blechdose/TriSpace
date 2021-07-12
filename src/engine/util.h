#ifndef _UTIL_H
#define _UTIL_H

#include <math.h>

typedef struct {
    float x;
    float y;
    float z;
} Vector3;

#define RAD_TO_DEG(rad) ((rad) * 180.0f / M_PI)

float clamp(float x, float min, float max);
float squaref(float a);
float distance3d(Vector3* a, Vector3* b);

#endif