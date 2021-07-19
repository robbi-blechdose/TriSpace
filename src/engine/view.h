#ifndef _VIEW_H
#define _VIEW_H

#include "includes/3dMath.h"
#include "GL/gl.h"

void initView(float fov, float winPersp[], float winOrtho[], float clipPersp[], float clipOrtho[]);
void setOrtho();
void setPerspective();

#endif