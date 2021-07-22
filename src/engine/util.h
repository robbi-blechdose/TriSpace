#ifndef _UTIL_H
#define _UTIL_H

#include <math.h>
#include "includes/3dMath.h"

#define RAD_TO_DEG(rad) ((rad) * 180.0f / M_PI)

float squaref(float a);
float distance3d(vec3* a, vec3* b);

#endif