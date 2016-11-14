#pragma once

/* STDLIBS */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* GLLIBS */
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>
#include <GL/gl.h>
#include <GL/glu.h>




renderObject *  renderNewObj(int nplots, int plotsize, GLfloat maxsize, GLfloat maxheight, GLfloat width);
void            renderAddSetToObj(renderObject * obj, GLfloat **values);

void            renderObj(renderObject * obj, int alphaing);
void            renderBar(GLfloat size, GLfloat h, GLfloat x, GLfloat z, GLfloat f);

renderObject *  renderGenObject(int sizez, int sizex, int shape, int values, int colour, int labels);
renderObject *  renderGenObjectShape(int shape, int sizez, int sizex);
renderObject *  renderGenObjectValues(renderObject * obj, int values);
renderObject *  renderGenObjectColour(renderObject * obj, int colour);
renderObject *  renderGenObjectLabels(renderObject * obj, int labels);
renderObject *  renderGenObjectMerge(renderObject *first, renderObject *second);
