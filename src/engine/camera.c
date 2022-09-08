#include "camera.h"

#include "GL/gl.h"

vec3 cameraPos;
float cameraRot[16];

void drawCamera()
{
	glLoadIdentity();
    glLoadMatrixf(cameraRot);
    glTranslatef(-cameraPos.x, -cameraPos.y, -cameraPos.z);
}

void setCameraPos(vec3 pos)
{
    cameraPos = pos;
}

void setCameraRot(quat rot)
{
    quatToMatrix(cameraRot, rot);
}