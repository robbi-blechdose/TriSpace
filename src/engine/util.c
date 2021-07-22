#include "util.h"

float squaref(float a)
{
    return a * a;
}

float distance3d(vec3* a, vec3* b)
{
    return sqrtf(squaref(b->x - a->x) + squaref(b->y - a->y) + squaref(b->z - a->z));
}