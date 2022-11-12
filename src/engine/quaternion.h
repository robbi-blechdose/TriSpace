#ifndef _QUATERNION_H
#define _QUATERNION_H

#include "includes/3dMath.h"

/**
 * Quaternion implementation using floats
 * 
 * Based on this article: https://www.cprogramming.com/tutorial/3d/quaternions.html
 * and the JMonkeyEngine 3 quaternion implementation
 **/

typedef struct {
    float w;

    float x;
    float y;
    float z;
} quat;

#define QUAT_INITIAL (quat) {1, 0, 0, 0}

quat normalizeQuat(quat q);

quat quatFromAxisAngle(vec3 axis, float angle);
quat quatFromAngles(vec3 angles);

void quatToAxisAngle(quat q, vec3* axis, float* angle);
void quatToMatrix(float* matrix, quat q);

quat multQuat(quat q1, quat q2);
vec3 multQuatVec3(quat q, vec3 v);

quat inverseQuat(quat q);

quat quatLookAt(vec3 pos, vec3 target, vec3 forward, vec3 up);

#endif