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

Vector3 difference3d(Vector3* a, Vector3* b)
{
    Vector3 ret;
    ret.x = b->x - a->x;
    ret.y = b->y - a->y;
    ret.z = b->z - a->z;
    return ret;
}

float length3d(Vector3* v)
{
    return sqrtf((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

void normalize(Vector3* v)
{
    float length = length3d(v);
    v->x /= length;
    v->y /= length;
    v->z /= length;
}