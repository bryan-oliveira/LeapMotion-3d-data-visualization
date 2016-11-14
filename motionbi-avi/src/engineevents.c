#include "engine.h"

int engineRunning;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * *  EVENTS MANAGEMENT  * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void engineEventLoop() {
    engineRunning = 1;
    SDL_Event event;
    
    while (engineRunning && SDL_WaitEvent(&event)) {
        SDL_Keycode key;
        switch (event.type) {
            case SDL_USEREVENT:
                if (engineCallbacks[event.user.code])        // is it allocated?
                    (*engineCallbacks[event.user.code])(event.user.data1,
                        event.user.data2);
                break;



            case SDL_KEYDOWN:
                if (event.key.repeat == 0)
                    keyboardEvtKeyboard(K_KDOWN, event.key.keysym);
                break;
            case SDL_KEYUP:
                keyboardEvtKeyboard(K_KUP, event.key.keysym);
                break;
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEMOTION:
            case SDL_MOUSEWHEEL:
                mouseHandleEvent(event);
                break;



            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) { /* Resized */
                    engineMainRender->winw = event.window.data1;
                    engineMainRender->winh = event.window.data2;
                }
                break;
            case SDL_QUIT:
                engineRunning = 0;
                break;
            default:
                break;
        }
    }
}


void engineAddEvent(int code, void (*func) (void*, void*)) {
    engineCallbacks[code] = func;
}

void engineRemoveEvent(int code) {
    engineCallbacks[code] = NULL;
}

void engineEmitEvent(int code, void* data1, void* data2) {
    SDL_Event event;
    
    event.type = SDL_USEREVENT;
    event.user.code = code;
    event.user.data1 = data1;
    event.user.data2 = data2;
    
    SDL_PushEvent(&event);
}



/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * EVENTS  * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void engineEvtRender(void* data1, void* data2) {
    /* call render */
    renderLoop((renderEnv*) data1);

    if (((renderEnv*) data1)->fullfps)
        engineEmitEvent(EV_RENDER, data1, NULL);
}

void engineEvtUpdater(void* data1, void* data2) {
    engineEnv * env = data1;
    int i;
    /* * * * * * * * CAMERAMOVE * * * * * * * */
    if (env->ecamera)
        engineUpdateCamera(env);

    /* * * * * * * OBJECT ENGINES * * * * * * */
    for (i = 0; i < *env->nobj; i++) {
        /* WORLD MOVE */
        engineUpdateWorld(env, i);
        
        /* SELECTION */
        engineUpdateSelection(env, i);
    }
}

void engineEvtCameraMove(void* data1, void* data2) {
    ev_cameramove * cm = data1;
    /*printf("EVT[CameraMove] %s : %.2f %.2f [%.2f, %.2f, %.2f]\n",
        cm->how == 0 ? "Rel" : "Abs",
        cm->angle, cm->distance, cm->x, cm->y, cm->z);*/
   
   /* TODO varitize this */
   /* normalized values */
   float kx = 0,    sx = 4.5,
         ky = 0,    sy = 4.5,
         kz = 0,    sz = 4.5;
   cm->x = kx + sx * cm->x;
   cm->y = ky + sy * cm->y;
   cm->z = kz + sz * cm->z;

    if (cm->angle == 0 && cm->distance == 0) { /* Moving by vector */
        if (cm->how == 0) { /* Relative */
            engineenv->ecamera->targetx += cm->x;
            engineenv->ecamera->targety += cm->y;
            engineenv->ecamera->targetz += cm->z;
        }
        else if (cm->how == 1) { /* Absolute */
            engineenv->ecamera->targetx = cm->x;
            engineenv->ecamera->targety = cm->y;
            engineenv->ecamera->targetz = cm->z;
        }
    }
    else if (cm->angle == 0 && cm->distance != 0) {/* Zoom */
        engineenv->ecamera->targetx *= cm->distance;
        engineenv->ecamera->targety *= cm->distance;
        engineenv->ecamera->targetz *= cm->distance;
    }
    else if (cm->angle != 0) { /* Rotate */
        if (cm->how == 0) { /* Relative */
            if (cm->x == 0 && cm->y == 1 && cm->z == 0) {
                double angle = cm->angle;

                double cx = engineenv->ecamera->targetx,
                cz = engineenv->ecamera->targetz;

                double radius = sqrt( cx * cx + cz * cz );
                double cang = atan2(-cz, cx);

                engineenv->ecamera->targetx = cos (cang + angle) * radius;
                engineenv->ecamera->targety = engineenv->ecamera->targety;
                engineenv->ecamera->targetz = -sin (cang + angle) * radius;
            }
        }
        else if (cm->how == 1) { /* Absolute */
            
        }
    }
    free(data1);
}

void engineEvtWorldRotate(void* data1, void* data2) {
    ev_worldrotate * wr = data1;
    if (wr->graf >= *engineenv->nobj)
        return;

    if (wr->how == 0) { /* Relative */
        engineenv->eworlds[wr->graf]->targetx = wr->x;
        engineenv->eworlds[wr->graf]->targety = wr->y;
        engineenv->eworlds[wr->graf]->targetz = wr->z;
        engineenv->eworlds[wr->graf]->targeta += wr->angle;
    }
    else if (wr->how == 1) { /* Absolute */
        engineenv->eworlds[wr->graf]->targetx = wr->x;
        engineenv->eworlds[wr->graf]->targety = wr->y;
        engineenv->eworlds[wr->graf]->targetz = wr->z;
        engineenv->eworlds[wr->graf]->targeta = wr->angle;
    }

    free(data1);
}

void engineEvtGraphVisibility(void* data1, void* data2) {
    ev_graphvisibility * gv = data1;
    if (gv->graf >= *engineenv->nobj)
        return;

    engineenv->mainrender->objects[gv->graf]->hide = !gv->visibility;

    free(data1);
}

void engineEvtSelection(void *data1, void *data2) {
    ev_selection *s = data1;
    if (s->graf >= *engineenv->nobj)
        return;
    engineenv->eselections[s->graf]->changed = 1;

    /* Set selection mode */
    switch (s->mode){
        case 1:
            engineenv->eselections[s->graf]->isselectedx = 0;
            engineenv->eselections[s->graf]->isselectedz = 0;
            break;
        case 2:
            engineenv->eselections[s->graf]->isselectedx = 1;
            engineenv->eselections[s->graf]->isselectedz = 0;
            break;
        case 3:
            engineenv->eselections[s->graf]->isselectedx = 0;
            engineenv->eselections[s->graf]->isselectedz = 1;
            break;
        case 4:
            engineenv->eselections[s->graf]->isselectedx = 1;
            engineenv->eselections[s->graf]->isselectedz = 1;
            break;
        case 0:
        default:
            break;
    }

    engineenv->eselections[s->graf]->actualx += s->hdir;
    engineenv->eselections[s->graf]->actualz += s->ddir;

    free(data1);
}

void engineEvtSelectMode(void *data1, void *data2) {
    ev_selectmode *sm = data1;
    if (sm->graf >= *engineenv->nobj)
        return;
    engineenv->eselections[sm->graf]->changed = 1;
    engineenv->eselections[sm->graf]->totransparent = sm->transparent;
    free(data1);
}


