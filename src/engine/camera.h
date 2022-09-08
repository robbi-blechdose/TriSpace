#ifndef _CAMERA_H
#define _CAMERA_H

#include "util.h"
#include "includes/3dMath.h"
#include "quaternion.h"

void drawCamera();
void setCameraPos(vec3 pos);
void setCameraRot(quat rot);

#endif