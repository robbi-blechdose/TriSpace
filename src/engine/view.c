#include "view.h"

mat4 mPerspective;
mat4 mOrtho;

mat4 ortho(float b, float t, float l, float r, float n, float f)
{ 
    mat4 matrix;
    matrix.d[0] = 2 / (r - l); 
    matrix.d[1] = 0; 
    matrix.d[2] = 0; 
    matrix.d[3] = 0; 
 
    matrix.d[4] = 0; 
    matrix.d[5] = 2 / (t - b); 
    matrix.d[6] = 0; 
    matrix.d[7] = 0; 
 
    matrix.d[8] = 0; 
    matrix.d[9] = 0; 
    matrix.d[10] = -2 / (f - n); 
    matrix.d[11] = 0; 
 
    matrix.d[12] = -(r + l) / (r - l); 
    matrix.d[13] = -(t + b) / (t - b); 
    matrix.d[14] = -(f + n) / (f - n); 
    matrix.d[15] = 1;
    
    return matrix;
}

void initView(float fov, float winPersp[], float winOrtho[], float clipPersp[], float clipOrtho[])
{
	mPerspective = perspective(fov, winPersp[0] / winPersp[1], clipPersp[0], clipPersp[1]);
	mOrtho = ortho(0, winOrtho[0], 0, winOrtho[1], clipOrtho[0], clipOrtho[1]);
}

void setOrtho()
{
    glViewport(0, 0, 240, 240);
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(mOrtho.d);
	glMatrixMode(GL_MODELVIEW);
}

void setPerspective()
{
    glViewport(0, 0, 240, 240 - 70);
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glLoadMatrixf(mPerspective.d);
	glMatrixMode(GL_MODELVIEW);
}