#include "engine.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * CAMERAMOVE  * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
engineCamera * engineNewCamera(double mindistance, double maxdistance,
    int time) {
    
    engineCamera * res = (engineCamera *) calloc(1, sizeof(engineCamera));
    
    res->actual = 0;
    
    res->mindistance = mindistance;
    res->minspeed = 1.0;
    res->maxdistance = maxdistance;
    
    res->fps = E_UPDATEFPS;
    res->time = time; /* milisenconds */
    res->max =  time / (time / res->fps);
    
    return res;
}

void engineUpdateCamera(void * data1) {
    engineEnv * env = data1;
    /* I'll check if I am too far from my target location */
    /* So, I'll get path */
    GLdouble pathx = env->ecamera->targetx - env->mainrender->lookat.eyex,
             pathy = env->ecamera->targety - env->mainrender->lookat.eyey,
             pathz = env->ecamera->targetz - env->mainrender->lookat.eyez;
    
    /* And my euclidian distance */
    GLdouble dist = sqrt(pathx * pathx + pathy * pathy + pathz * pathz);

    /* Am I? */
    if (dist < env->ecamera->mindistance && !env->ecamera->actual) { /* Oh! I'm too close! */
        /* I can move directly to my target and user may not notice :) */
        
        env->mainrender->lookat.eyex = env->ecamera->targetx;
        env->mainrender->lookat.eyey = env->ecamera->targety;
        env->mainrender->lookat.eyez = env->ecamera->targetz;
        
        /* Avoid any GL_Smooth trip */
        env->ecamera->actual = 0;
        /* Good! */
    } else { /* Sure, blame it on your ISP! I'am too far*/
        /* What can I do? GL_Smooth transition to the target, of course */
        
        /* But just before that, I'll check if my target just move far far away */
        GLdouble targetdistance = sqrt(
            pow(env->ecamera->targetx - env->ecamera->lastx,2) +
            pow(env->ecamera->targety - env->ecamera->lasty,2) +
            pow(env->ecamera->targetz - env->ecamera->lastz,2));
        
        if (/*targetdistance > env->ecamera->maxdistance*/ dist > env->ecamera->lastd) {
            /* Where is it? It moved too far away :( */

            /* It's ok...! I'll give him some more time to get there */
            env->ecamera->actual /=  2;
        }
        
        /* What do I need? I need the direction to the target */
        GLfloat dirx = pathx / dist,
                diry = pathy / dist,
                dirz = pathz / dist;
        
        /* Now, depending on how far I am, I can calculate how fast should I
            move just to make sure I am there quick enough */
        GLfloat speed = dist / (env->ecamera->max - env->ecamera->actual*1.0);
        
        /* Ok. But.. Wait! Is that speed enough? */
        /*if (speed < env->ecamera->minspeed)
            return; /* // I just want to workaround divide by zero =P */
        
        /* Start the game already! Let's move! Muahaha */
        env->mainrender->lookat.eyex += dirx * speed;
        env->mainrender->lookat.eyey += diry * speed;
        env->mainrender->lookat.eyez += dirz * speed;
        
        /* Let's prepare thing to the next iteration */
        env->ecamera->actual++;
        if (env->ecamera->actual >= env->ecamera->max) { /* Just finish the trip trough the spaaaace */
            /* Cleaning up some potatoes around there */
            
            env->ecamera->actual = 0;
            /* It was just one potato */
        
            /* It is good to be the King! Phew! */
        }        
    }
    
    env->ecamera->lastd = dist;
    
    /* I forgot things too fast. I save my current target, then */
    env->ecamera->lastx = env->ecamera->targetx;
    env->ecamera->lasty = env->ecamera->targety;
    env->ecamera->lastz = env->ecamera->targetz;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * *  WORLDMOVE  * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
engineWorld * engineNewWorld(double mindistance, double maxdistance, int time) {
    
    engineWorld * res = (engineWorld *) calloc(1, sizeof(engineWorld));
    
    res->actual = 0;
    
    res->mindistance = mindistance;
    res->minspeed = 1.0;
    res->maxdistance = maxdistance;
    
    res->fps = E_UPDATEFPS;
    res->time = time; /* milisenconds */
    res->max =  time / (time / res->fps);
    
    res->targetx = 0;
    res->targety = 1;
    res->targetz = 0;
    res->targeta = 0;
    
    return res;
}

void engineUpdateWorld(void * data1, int obj) {
    engineEnv * env = data1;
    renderTMatrix * tm = &(env->mainrender->objects[obj]->tmatrix);
    engineWorld * ew = env->eworlds[obj];

    //ew->targeta = fmod(ew->targeta + 2 * PI, 4 * PI) - 2 * PI;
    //ew->targeta = fmod(ew->targeta, 2* PI);
    /* TODO: fix range of angle */

    GLdouble dist = fabs(tm->angle - ew->targeta);
    if (dist < ew->mindistance && !ew->actual) {
        tm->angle = ew->targeta;
        ew->actual = 0;

    } else {
        GLdouble targetdistance = fabs(ew->targeta - ew->lasta);

        if (dist > ew->lastd) {
            ew->actual /=  2;
        }

        GLfloat speed = (ew->targeta - tm->angle) / (ew->max - ew->actual*1.0);
        tm->angle += speed;

        ew->actual++;
        if (ew->actual >= ew->max) {
            ew->actual = 0;
        }
    }

    ew->lastd = dist;
    ew->lasta = ew->targeta;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * *  SELECTION  * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
engineSelection * engineNewSelection() {
    engineSelection * res = (engineSelection *) calloc(1, sizeof(engineSelection));

    res->changed = 1;
    /* Init the default values */

    return res;
}

void engineUpdateSelection(void * data1, int obj) {
    engineEnv * env = data1;
    if (!env->eselections[obj]->changed)
        return;
    
    /* Check for boundaries */
    if (env->eselections[obj]->actualx >= env->mainrender->objects[obj]->xnum)
        env->eselections[obj]->actualx = env->mainrender->objects[obj]->xnum - 1;
    if (env->eselections[obj]->actualx < 0)
        env->eselections[obj]->actualx = 0;
    if (env->eselections[obj]->actualz >= env->mainrender->objects[obj]->znum)
        env->eselections[obj]->actualz = env->mainrender->objects[obj]->znum - 1;
    if (env->eselections[obj]->actualz < 0)
        env->eselections[obj]->actualz = 0;

    int x, z;
    for (x = 0; x < env->mainrender->objects[obj]->xnum; x++) {
    
        for (z = 0; z < env->mainrender->objects[obj]->znum; z++) {
            if (env->eselections[obj]->isselectedx && env->eselections[obj]->isselectedz) {
                /* when both are selected */
                if (x == env->eselections[obj]->actualx &&
                    z == env->eselections[obj]->actualz)
                    env->mainrender->objects[obj]->states[z][x] = R_SELECTED;
                else
                    env->mainrender->objects[obj]->states[z][x] =
                        env->eselections[obj]->totransparent ? R_UNSELECTED : R_NADIR;
            }
            else if (env->eselections[obj]->isselectedx) {
                /* when x is selected */
                if (x == env->eselections[obj]->actualx)
                    env->mainrender->objects[obj]->states[z][x] = R_SELECTED;
                else
                    env->mainrender->objects[obj]->states[z][x] =
                        env->eselections[obj]->totransparent ? R_UNSELECTED : R_NADIR;
            }
            else if (env->eselections[obj]->isselectedz) {
                /* when z is selected */
                if (z == env->eselections[obj]->actualz)
                    env->mainrender->objects[obj]->states[z][x] = R_SELECTED;
                else
                    env->mainrender->objects[obj]->states[z][x] =
                        env->eselections[obj]->totransparent ? R_UNSELECTED : R_NADIR;
            }
            else if (!env->eselections[obj]->isselectedx && !env->eselections[obj]->isselectedz) {
                /* default */
                env->mainrender->objects[obj]->states[z][x] = R_DEFAULT;
            }
            else {
                /* unselected ones */
                env->mainrender->objects[obj]->states[z][x] = R_DEFAULT;
            }
        }
    }
    
    env->eselections[obj]->changed = 0;
}
