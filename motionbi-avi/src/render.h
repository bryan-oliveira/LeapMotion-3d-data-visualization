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



/* DEFINITIONS */
#define R_WWIDTH    640
#define R_WHEIGHT   480

#define R_DEFwintitle   "GPRS"
#define R_DEFlookat     {0, 200, 100, 0, 0, 0, 0, 1, 0}
#define R_DEFambient    {0.0f, 0.0f,  0.0f, 1.0f}
/*#define R_DEFclear      {1.0f, 1.0f,  1.0f, 1.0f}*/
#define R_DEFclear      {0.0f, 0.0f,  0.0f, 1.0f}/**/
#define R_DEFobjs       10
#define R_DEFdrawxyz    0
#define R_DEFfps        60
#define R_DEFfullfps    1
#define R_DEFwireframe  0
#define R_DEFlinew      1.0

#define R_DEFmaxsets    14


/* DATATYPE */
/* Simple 3d point */
typedef struct renderPoint3d_t {
    GLfloat     x, y, z;
} renderPoint3d;
/* And a simple rgba colour struct */
typedef struct renderColour_t {           /* colour value */
    GLfloat     r, g, b, a;
} renderColour;
/* Actual look at point */
typedef struct renderLookat_t {
    GLdouble    eyex,      eyey,       eyez,
                centerx,   centery,    centerz,
                upx,       upy,        upz;
} renderLookat;


/* A material */
typedef struct renderMaterial_t {
    renderColour        ambient,            /* ambient material colour */
                        emission,           /* emission material colour */
                        diffuse,            /* diffuse material colour */
                        specular;           /* specular material colour */
    GLfloat             shininess;          /* shininess material colour */
} renderMaterial;

/* Transform matrix values */
typedef struct _renderTMatrix {
    renderPoint3d       translate,
                        scale,
                        rotate;
    GLfloat             angle;
} renderTMatrix;


/* A bar state */
typedef enum _renderState {
    R_DEFAULT       = 0,
    R_SELECTED      = 1,
    R_NADIR         = 2,
    R_UNSELECTED    = 3,
} renderState;

/* Titles */
typedef struct _renderTitles {
    char                  **xlabels,            /* Legends */
                          **zlabels,
                           *xaxislabel,
                           *yaxislabel,
                           *zaxislabel;

    GLuint                 *xtextures,          /* Textures */
                           *ztextures,
                            xaxistexture,
                            yaxistexture,
                            zaxistexture;

    double                 *xratios,            /* Ratios */
                           *zratios,
                            xaxisratio,
                            yaxisratio,
                            zaxisratio;

    renderMaterial          mataxis,            /* Colours */
                            matlabels,
                            matbars;
   /* Bars labels */
} renderTitles;

/* A set */
typedef struct _renderSet {
    GLfloat               **values;             /* The values and sizes */
    renderMaterial          mat;                /* The material of the set */
    renderColour            colour;             /* The set's colour */
    int                     show;               /* If it is to show */
} renderSet;

/* A reder unit */
typedef struct _renderObject {
    unsigned int            id;                 /* An identifier */
    renderTMatrix           tmatrix;            /* Transform matrix */
    
    GLint                   hide;               /* When set to 1, the graph is hidden*/

    GLint                   drawfloor;          /* If it is to draw or not the floor */
    
    renderMaterial          matdefault,         /* The materials */
                            matfloor;
    
    renderColour            colour;             /* The base colour */
    GLfloat                 highlight,          /* How much to bright the selected ones */
                            nadir;              /* The transperency of unselected ones */
    
    renderState           **states;             /* States of each bar */
    
    GLfloat               **values;             /* The sum of values and sizes */
    GLint                   xnum,               /* Number of values per plot */
                            znum;               /* Number of plots */
    
    renderSet               sets[R_DEFmaxsets]; /* The subsets */
    int                     nsets;              /* Number of sets used */
    
    renderTitles            titles;             /* The titles */
    char                  **xlegends,           /* Legends */
                          **zlegends;
    
    GLuint                 *xlegendtexture,
                           *zlegendtexture;

    char                   *xaxislabel,
                           *yaxislabel,
                           *zaxislabel;
    GLuint                 xaxislabeltexture,
                           yaxislabeltexture,
                           zaxislabeltexture;


    GLfloat                 minx, maxx,         /* Boundaries */
                            miny, maxy,
                            minz, maxz;
    
    GLfloat                 width,              /* width of bars */
                            higher,             /* to normalize the height */
                            gap,                /* the space between two bars */
                            inc,                /* incremente on each bar position */
                            xinit,              /* start of bars on xx */
                            zinit;              /* start of bars on zz */
} renderObject;



/* Render Environment */
typedef struct _renderEnv {
    renderObject          **objects;
    unsigned int            nobj;
    
    SDL_Window             *window;
    SDL_GLContext           glcontext;
    
    renderLookat            lookat;             /* Camera */
    GLfloat                 winw,               /* Window size */
                            winh;
    /* Lights */
    renderColour            ambientlight;       /* Lights */

    renderColour            clearcolour;        /* Clear Colour */

    /* Configs */
    unsigned char           antialiasing;
    unsigned char           drawxyz;
    unsigned char           wireframe;
    unsigned int            fps;
    unsigned char           fullfps;
    GLfloat                 linewidth;
    char                    windowtitle[50];
} renderEnv;


#include "renderobject.h"

/* GLOABALS */
int randInitColor;

/* HEADERS */
renderEnv *     renderInit();
void            renderStop();
void            renderLoop();

void            render_drawXYZ(GLfloat size);
void            renderLightning(renderEnv * env);
void            renderSetMaterial(renderMaterial m, GLfloat highlight, GLfloat alpha);

void            renderAddObject(renderEnv * env, renderObject * obj);


GLuint          renderTextTexture(char *text, double * ratio, int mipmaps);





renderMaterial  renderMaterialPreset(int id);
int             renderMaterialHasAlpha(renderMaterial mat);
void            glLight3f(GLenum light, GLenum pname, GLfloat a, GLfloat b, GLfloat c);
void            glLight4f(GLenum light, GLenum pname, GLfloat a, GLfloat b, GLfloat c, GLfloat d);
void            glMaterial3f(GLenum light, GLenum pname, GLfloat a, GLfloat b, GLfloat c);
void            glMaterial4f(GLenum light, GLenum pname, GLfloat a, GLfloat b, GLfloat c, GLfloat d);
GLfloat *       renderNormalizevf(GLfloat * v);
void            renderGenRandomValues(renderObject * obj, int x, int y, int shape);
renderColour    renderHSVtoRGB(double h, double s, double v, double alpha);







