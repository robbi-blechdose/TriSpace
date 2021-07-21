#ifndef _UTIL_H
#define _UTIL_H

#include <math.h>
#include "includes/3dMath.h"

typedef struct {
    float x;
    float y;
    float z;
} Vector3;

typedef struct {
    float x;
    float y;
} Vector2;

#define RAD_TO_DEG(rad) ((rad) * 180.0f / M_PI)

float clamp(float x, float min, float max);
float squaref(float a);
float distance3d(Vector3* a, Vector3* b);
Vector3 difference3d(Vector3* a, Vector3* b);
float length3d(Vector3* v);
void normalize(Vector3* v);

#endif