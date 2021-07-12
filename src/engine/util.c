#include "util.h"

float clamp(float x, float min, float max)
{
    if(x < min)
    {
        return min;
    }
    else if(x > max)
    {
        return max;
    }
    return x;
}

float squaref(float a)
{
    return a * a;
}

float distance3d(Vector3* a, Vector3* b)
{
    return sqrtf(squaref(b->x - a->x) + squaref(b->y - a->y) + squaref(b->z - a->z));
}