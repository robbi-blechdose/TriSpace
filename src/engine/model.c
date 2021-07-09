#include "model.h"
#include "includes/tobjparse.h"

model loadModel(const char* path)
{
    objraw omodel;
	model m = initmodel();
	omodel = tobj_load(path);

	if(!omodel.positions)
    {
		printf("No positions in model \"%s\".\n", path);
    }
	m = tobj_tomodel(&omodel);
    freeobjraw(&omodel);
	//printf("Has %ld points.\n", m.npoints);
    return m;
}

GLuint createModelDisplayList(model* model)
{
	GLuint ret = 0;
	if(!model->d)
    {
		return 0;
    }
	ret = glGenLists(1);
	glNewList(ret, GL_COMPILE);
	glBegin(GL_TRIANGLES);
	for(uint i = 0; i < model->npoints; i++)
    {
        //Colors
		if(model->c)
        {
			glColor3f(model->c[i].d[0], model->c[i].d[1], model->c[i].d[2]);
		}
        //Texcoords
		if(model->t)
        {
			glTexCoord2f(model->t[i].d[0], model->t[i].d[1]);
        }
        //Normals
		if(model->n)
        {
			glNormal3f(model->n[i].d[0], model->n[i].d[1], model->n[i].d[2]);
        }
		glVertex3f(model->d[i].d[0], model->d[i].d[1], model->d[i].d[2]);
	}
	glEnd();
	glEndList();
	return ret;
}

GLuint loadModelList(const char* path)
{
	model temp = loadModel(path);
	GLuint ret = createModelDisplayList(&temp);
	freemodel(&temp);
    return ret;
}