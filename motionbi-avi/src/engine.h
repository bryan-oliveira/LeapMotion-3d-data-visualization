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

#include "engineevents.h"
#include "enginetransitions.h"

#include "render.h"
#include "legacy.h"
#include "events.h"

/* DEFINITIONS */
#define E_UPDATEFPS 20.0
#define PI 3.14159265359

/* DATATYPE */
typedef struct engineEnv_t {
    renderEnv *         mainrender;
    
    /* Global engines */
    engineCamera *      ecamera;
    
    /* Per object engines */
    engineWorld *       eworlds[R_DEFobjs];
    engineSelection *   eselections[R_DEFobjs];
    
    /* Shortcut to number of objects */
    int *               nobj;

}  engineEnv;

/* GLOABALS */
renderEnv *             engineMainRender;
engineEnv *             engineenv;

/* HEADERS */
void                    engineInit();
void                    engineRun();
void                    engineStop();

engineEnv *             engineNewEnv();
void                    engineAddObject(engineEnv * env, renderObject * obj);

Uint32                  engineTimerRenderEmiter(Uint32 interval, void* param);
Uint32                  engineTimerUpdate(Uint32 interval, void* param);

ev_cameramove *         engineNewCM(unsigned how, double angle, double distance, double x, double y, double z);
ev_worldrotate *        engineNewWR(unsigned how, double angle, double x, double y, double z, int graf);
ev_selection *          engineNewS(int mode, int hdir, int ddir, int graf);
ev_selectmode *         engineNewSM(int totransparent, int graf);
ev_graphvisibility *    engineNewGV(int visibility, int graf);

void                    engineGetWindowSize (int *width, int *height);

renderObject *          engineNewSampledObject(int witch);



