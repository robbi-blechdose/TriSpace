#include "camera.h"

#include "GL/gl.h"

vec3 cameraPos;
//x = pitch, y = yaw, z = roll
vec3 cameraRot;

void drawCamera()
{
	glLoadIdentity();
    glRotatef(RAD_TO_DEG(cameraRot.x), 1, 0, 0);
    glRotatef(RAD_TO_DEG(cameraRot.y), 0, 1, 0);
    glRotatef(RAD_TO_DEG(cameraRot.z), 0, 0, 1);
    glTranslatef(-cameraPos.x, -cameraPos.y, -cameraPos.z);
}

void setCameraPos(vec3 pos)
{
    cameraPos = pos;
}

void setCameraRot(vec3 rot)
{
    cameraRot = rot;
}