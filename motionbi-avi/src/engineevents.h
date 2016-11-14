#pragma once

/* STDLIBS */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* GLLIBS */
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <GL/gl.h>
#include <GL/glu.h>

/* DEFINITIONS */
#define E_MAXEVENTS 256

/* GLOABALS */
void (*engineCallbacks[E_MAXEVENTS]) (void* data1, void* data2);



/* HEADERS */
void engineEventLoop();
void engineAddEvent(int code, void (*func) (void*, void*));
void engineRemoveEvent(int code);
void engineEmitEvent(int code, void* data1, void* data2);

void engineEvtRender(void* data1, void* data2);
void engineEvtUpdater(void* data1, void* data2);
void engineEvtKeyboard(int type, SDL_Keysym keysym);

void engineEvtCameraMove(void* data1, void* data2);
void engineEvtWorldRotate(void* data1, void* data2);
void engineEvtSelection(void *data1, void *data2);
void engineEvtDepthSelection(void *data1, void *data2);
void engineEvtHorSelection(void *data1, void *data2);
void engineEvtGraphVisibility(void* data1, void* data2);
void engineEvtSelectMode(void *data1, void *data2);


