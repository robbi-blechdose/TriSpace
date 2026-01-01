#include "backgroundstars.h"

#include "GL/gl.h"

#include "fk-engine-core/util.h"

GLuint backgroundStarList;

void initBackgroundStars()
{
	backgroundStarList = glGenLists(1);
	glNewList(backgroundStarList, GL_COMPILE);
	glBegin(GL_POINTS);
	for(uint16_t i = 0; i < NUM_BACKGROUND_STARS; i++)
    {
        vec3 point = getRandomSpherePoint((vec3) {0, 0, 0}, 50);
		glVertex3f(point.x, point.y, point.z);
	}
	glEnd();
	glEndList();
}

void drawBackgroundStars()
{
    glColor3f(1, 1, 0.95f);
    glCallList(backgroundStarList);
    glColor3f(1, 1, 1);
    //Clear the depth buffer so everything else is drawn on top of the stars
    glClear(GL_DEPTH_BUFFER_BIT);
}