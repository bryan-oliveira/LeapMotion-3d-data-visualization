#include "engine.h"

void engineInit() {
    engineenv = engineNewEnv();

    /* legacy mode */
    engineMainRender = engineenv->mainrender;

    /* Install callbacks */
    engineAddEvent(EV_RENDER, engineEvtRender);
    engineAddEvent(EV_UPDATE, engineEvtUpdater);

    engineAddEvent(EV_CAMERAMOVE, engineEvtCameraMove);
    engineAddEvent(EV_WORLDROTATE, engineEvtWorldRotate);
    engineAddEvent(EV_SELECTION, engineEvtSelection);
    engineAddEvent(EV_GRAPHVISIBILITY, engineEvtGraphVisibility);

    engineAddEvent(EV_SELECTMODE, engineEvtSelectMode);

    /* Add objects */
    renderObject *first = engineNewSampledObject(1);
    renderObject *second = engineNewSampledObject(11);
    renderObject *third = renderGenObjectMerge(first, second);

    engineAddObject(engineenv, first);
    engineAddObject(engineenv, second);
    engineAddObject(engineenv, third);

    /* legacy mode */
    /*enginecamera = engineenv->ecamera;
    engineworld = engineenv->eworlds[0];
    engineselection = engineenv->eselections[0];*/
}

void engineRun() {
    /* Init timer */
    SDL_AddTimer(1000/engineMainRender->fps, engineTimerRenderEmiter,
        &(engineMainRender->fullfps));
    SDL_AddTimer(engineenv->ecamera->time / engineenv->ecamera->fps, engineTimerUpdate, 0);
    
    /* First render */
    engineEmitEvent(EV_RENDER, engineMainRender, NULL);
    
    engineEventLoop();
}

void engineStop() {
    
}

engineEnv * engineNewEnv() {
    engineEnv * res = (engineEnv *) calloc(1, sizeof(engineEnv));

    /* Init render */
    res->mainrender = renderInit();
    res->nobj = &res->mainrender->nobj;

    /* Init global transitions engines */
    res->ecamera = engineNewCamera(0.5, 2.0, 150);
    /* Go to init position */
    engineEmitEvent(EV_CAMERAMOVE, engineNewCM(1, 0.0, 0.0, 1, 1, 1), NULL);

    return res;
}

void engineAddObject(engineEnv * env, renderObject * obj) {
    renderAddObject(env->mainrender, obj);

    /* Init object dependent engines */
    env->eworlds[obj->id] = engineNewWorld(PI/10.0, 0.005, 150);
    env->eselections[obj->id] = engineNewSelection();
    engineEmitEvent(EV_SELECTMODE, engineNewSM(1, obj->id), NULL);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * TIMERS  * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
Uint32 engineTimerRenderEmiter(Uint32 interval, void* param) {
    if (!(*(char*)param))
        engineEmitEvent(EV_RENDER, engineMainRender, NULL);
    return interval;
}

Uint32 engineTimerUpdate(Uint32 interval, void* param) {
    engineEmitEvent(EV_UPDATE, engineenv, NULL);
    return interval;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * *  TOOLS  * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
ev_cameramove * engineNewCM(unsigned how, double angle, double distance,
    double x, double y, double z) {
    ev_cameramove * res = calloc(1, sizeof(ev_cameramove));
    
    res->how = how;
    res->angle = angle;
    res->distance = distance;
    res->x = x; res->y = y; res->z = z;
    
    return res;
}

ev_graphvisibility * engineNewGV(int visibility, int graf) {
    ev_graphvisibility * res = (ev_graphvisibility *)calloc(1, sizeof(ev_graphvisibility));
    
    res->visibility = visibility;
    res->graf = graf;
    
    return res;
}

ev_worldrotate * engineNewWR(unsigned how, double angle, double x, double y, double z, int graf) {
    ev_worldrotate * res = calloc(1, sizeof(ev_worldrotate));
    
    res->how = how;
    res->angle = angle;
    res->x = x; res->y = y; res->z = z;
    res->graf = graf;
    
    return res;
}

ev_selection * engineNewS(int mode, int hdir, int ddir, int graf) {
    ev_selection * res = calloc(1, sizeof(ev_selection));
    res->mode = mode;
    res->hdir = hdir;
    res->ddir = ddir;
    res->graf = graf;
    return res;
}

ev_selectmode * engineNewSM(int totransparent, int graf) {
    ev_selectmode * res = calloc(1, sizeof(ev_selectmode));
    res->transparent = totransparent;
    res->graf = graf;
    return res;
}

void engineGetWindowSize (int *width, int *height) {
    SDL_GetWindowSize (engineenv->mainrender->window, width, height);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * SAMPLES * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
renderObject * engineNewSampledObject(int witch) {
    renderObject * newobj;
    switch (witch) {
        case 1: /* with three subsets */
            newobj = renderGenObject(10, 10, 0, 0, 0, 0);
            renderGenObjectValues(newobj, 1);
            renderGenObjectValues(newobj, 2);
            newobj->tmatrix.translate.x = -2.0;
            break;
        case 11: /* with three subsets */
            newobj = renderGenObject(10, 10, 0, 0, 0, 0);
            renderGenObjectValues(newobj, 1);
            renderGenObjectValues(newobj, 2);
            newobj->tmatrix.translate.x = 2.0;
            break;
        case 2: /* with three subsets, 20x20 */
            newobj = renderGenObject(20, 20, 0, 0, 0, 0);
            renderGenObjectValues(newobj, 1);
            renderGenObjectValues(newobj, 2);
            break;
        case 0:     /* Default */
        default:
            newobj = renderGenObject(10, 10, 0, 0, 0, 0);
            break;
    }
    return newobj;
}
