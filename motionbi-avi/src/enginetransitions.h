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

/* DATATYPE */
typedef struct engineCamera_t {
    GLdouble targetx,
             targety,
             targetz;
    
    GLdouble lastd,
             lastx,
             lasty,
             lastz;
    
    int max;
    int actual;
    
    GLdouble mindistance;
    GLdouble minspeed;
    
    GLdouble maxdistance;

    GLdouble time,
             fps;

}  engineCamera;
typedef struct engineWorld_t {
    GLdouble targetx,
             targety,
             targetz,
             targeta;
    
    GLdouble lastd,
             lastx,
             lasty,
             lastz,
             lasta;
    
    int max;
    int actual;
    
    GLdouble mindistance;
    GLdouble minspeed;
    
    GLdouble maxdistance;

    int      time,
             fps;

}  engineWorld;
typedef struct engineSelection_t {
    int     changed;

    int     actualx,
            actualz,
            isselectedx,
            isselectedz;
    int     totransparent;  /* 1 to totransparent, 0 to nadir */
}  engineSelection;

/* GLOABALS */
/*engineCamera *      enginecamera;
engineWorld *       engineworld;
engineSelection *   engineselection;*/


engineCamera *      engineNewCamera(double mindistance, double maxdistance, int time);
void                engineUpdateCamera(void * data1);
engineWorld *       engineNewWorld(double mindistance, double maxdistance, int time);
void                engineUpdateWorld(void * data1, int obj);
engineSelection *   engineNewSelection();
void                engineUpdateSelection(void * data1, int obj);

