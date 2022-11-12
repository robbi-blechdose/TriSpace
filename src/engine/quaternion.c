#include "quaternion.h"

#include <math.h>

float norm(quat q)
{
    return (q.w * q.w) +
           (q.x * q.x) +
           (q.y * q.y) +
           (q.z * q.z);
}

quat normalizeQuat(quat q)
{
    float magnitude = sqrtf(norm(q));
    return (quat) {.w = q.w / magnitude,
                   .x = q.x / magnitude,
                   .y = q.y / magnitude,
                   .z = q.z / magnitude};
}

quat quatFromAxisAngle(vec3 axis, float angle)
{
    return (quat) {.w = cosf(angle / 2),
                   .x = axis.x * sinf(angle / 2),
                   .y = axis.y * sinf(angle / 2),
                   .z = axis.z * sinf(angle / 2)};
}

quat quatFromAngles(vec3 angles)
{
    angles.x *= 0.5f;
    angles.y *= 0.5f;
    angles.z *= 0.5f;

    float sinX = sinf(angles.x);
    float cosX = cosf(angles.x);
    float cosYcosZ = cosf(angles.y) * cosf(angles.z);
    float sinYsinZ = sinf(angles.y) * sinf(angles.z);
    float cosYsinZ = cosf(angles.y) * sinf(angles.z);
    float sinYcosZ = sinf(angles.y) * cosf(angles.z);

    quat q;
    q.w = (cosYcosZ * cosX - sinYsinZ * sinX);
    q.x = (cosYcosZ * sinX + sinYsinZ * cosX);
    q.y = (sinYcosZ * cosX + cosYsinZ * sinX);
    q.z = (cosYsinZ * cosX - sinYcosZ * sinX);

    return normalizeQuat(q);
}

void quatToAxisAngle(quat q, vec3* axis, float* angle)
{
    if(q.w > 1)
    {
        q = normalizeQuat(q);
    }

    *angle = 2 * acosf(q.w);
    float s = sqrtf(1 - q.w * q.w);

    if(s < 0.001f)
    {
        axis->x = 1;
        axis->y = 0;
        axis->z = 0;
    }
    else
    {
        axis->x = q.x / s;
        axis->y = q.y / s;
        axis->z = q.z / s;
    }
}

void quatToMatrix(float* matrix, quat q)
{
    matrix[ 0] = 1 - (2 * q.y * q.y) - (2 * q.z * q.z);
    matrix[ 1] = (2 * q.x * q.y) - 2 * (q.w * q.z);
    matrix[ 2] = (2 * q.x * q.z) + 2 * (q.w * q.y);
    matrix[ 3] = 0;

    matrix[ 4] = (2 * q.x * q.y) + (2 * q.w * q.z);
    matrix[ 5] = 1 - (2 * q.x * q.x) - (2 * q.z * q.z);
    matrix[ 6] = (2 * q.y * q.z) - (2 * q.w * q.x);
    matrix[ 7] = 0;

    matrix[ 8] = (2 * q.x * q.z) - (2 * q.w * q.y);
    matrix[ 9] = (2 * q.y * q.z) + (2 * q.w * q.x);
    matrix[10] = 1 - (2 * q.x * q.x) - (2 * q.y * q.y);
    matrix[11] = 0;

    matrix[12] = 0;
    matrix[13] = 0;
    matrix[14] = 0;
    matrix[15] = 1;
}

quat multQuat(quat q1, quat q2)
{
    return (quat) {.w = ((q1.w * q2.w) - (q1.x * q2.x) - (q1.y * q2.y) - (q1.z * q2.z)),
                   .x = ((q1.w * q2.x) + (q1.x * q2.w) + (q1.y * q2.z) - (q1.z * q2.y)),
                   .y = ((q1.w * q2.y) - (q1.x * q2.z) + (q1.y * q2.w) + (q1.z * q2.x)),
                   .z = ((q1.w * q2.z) + (q1.x * q2.y) - (q1.y * q2.x) + (q1.z * q2.w))};
}

vec3 multQuatVec3(quat q, vec3 v)
{
    vec3 ret;
    ret.x = (q.w * q.w * v.x) + (2 * q.y * q.w * v.z) - (2 * q.z * q.w * v.y) + (q.x * q.x * v.x)
            + (2 * q.y * q.x * v.y) + (2 * q.z * q.x * v.z) - (q.z * q.z * v.x) - (q.y * q.y * v.x);
    ret.y = (2 * q.x * q.y * v.x) + (q.y * q.y * v.y) + (2 * q.z * q.y * v.z) + (2 * q.w * q.z * v.x)
            - (q.z * q.z * v.y) + (q.w * q.w * v.y) - (2 * q.x * q.w * v.z) - (q.x * q.x * v.y);
    ret.z = (2 * q.x * q.z * v.x) + (2 * q.y * q.z * v.y) + (q.z * q.z * v.z) - (2 * q.w * q.y * v.x)
            - (q.y * q.y * v.z) + (2 * q.w * q.x * v.y) - (q.x * q.x * v.z) + (q.w * q.w * v.z);
    return ret;
}

quat inverseQuat(quat q)
{
    float n = norm(q);
    if(n > 0)
    {
        q.w *= 1 / n;
        q.x *= -(1 / n);
        q.y *= -(1 / n);
        q.z *= -(1 / n);
    }

    return q;
}

quat quatLookAt(vec3 pos, vec3 target, vec3 forward, vec3 up)
{
    vec3 diff = normalizev3(subv3(target, pos));

    vec3 axis = normalizev3(crossv3(forward, diff));
    if(lengthv3(axis) <= 0.000001f)
    {
        axis = up;
    }

    float angle = acosf(dotv3(forward, diff));

    return quatFromAxisAngle(axis, angle);
}