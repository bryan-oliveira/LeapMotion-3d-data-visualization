#include "legacy.h"

#define PI 3.14159265359

int modshift,
    modctrl,
    modalt;
int selgraf;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * KEYBOARD  * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void keyboardEvtKeyboard(int type, SDL_Keysym keysym) {
    if (type == 1 && modalt)
        printf("Key%s: %d\n", type == 1? "down" : "up", keysym.sym);/**/
    
    if (keysym.sym == 1073742049 || keysym.sym == 1073742053) /* Shift */
        modshift = type;
    if (keysym.sym == 1073742048 || keysym.sym == 1073742052) /* Control */
        modctrl = type;
    if (keysym.sym == 1073742050 || keysym.sym == 1073742050) /* Alt */
        modalt = type;
    
    if (keysym.sym == 27)
        exit(0); /* TODO elegant way to go out */
    
    /* CAMERA MOVE */
    if (keysym.sym == 97 && type == K_KDOWN) { // A
        if (modshift)
            engineEmitEvent(EV_CAMERAMOVE,
                engineNewCM(1, 0.0, 0.0, 0, 1, 1),
                NULL);
        else
            engineEmitEvent(EV_CAMERAMOVE,
                engineNewCM(1, 0.0, 0.0, 1, 1, 1),
                NULL);
        
    }
    if (keysym.sym == 115 && type == K_KDOWN) { // S
        if (modshift)
            engineEmitEvent(EV_CAMERAMOVE,
                engineNewCM(1, 0.0, 0.0, 1, 1, 0),
                NULL);
        else
            engineEmitEvent(EV_CAMERAMOVE,
                engineNewCM(1, 0.0, 0.0, 1, 1, -1),
                NULL);
        
    }
    if (keysym.sym == 100 && type == K_KDOWN) { // D
        if (modshift)
            engineEmitEvent(EV_CAMERAMOVE,
                engineNewCM(1, 0.0, 0.0, 0, 1, -1),
                NULL);
        else
            engineEmitEvent(EV_CAMERAMOVE,
                engineNewCM(1, 0.0, 0.0, -1, 1, -1),
                NULL);
        
    }
    if (keysym.sym == 102 && type == K_KDOWN) { // F
        if (modshift)
            engineEmitEvent(EV_CAMERAMOVE,
                engineNewCM(1, 0.0, 0.0, -1, 1, 0),
                NULL);
        else
            engineEmitEvent(EV_CAMERAMOVE,
                engineNewCM(1, 0.0, 0.0, -1, 1, 1),
                NULL);
        
    }
    
    /* WORLD MOVE */
    if (keysym.sym == 122 && type == K_KDOWN) { // Z
        if (modctrl) engineEmitEvent(EV_WORLDROTATE, engineNewWR(0, PI/8.0, 0, 1, 0, selgraf), NULL);
        else engineEmitEvent(EV_WORLDROTATE, engineNewWR(1, 0, 0, 1, 0, selgraf), NULL);
    }
    if (keysym.sym == 120 && type == K_KDOWN) { // X
        if (modctrl) engineEmitEvent(EV_WORLDROTATE, engineNewWR(0, PI/4.0, 0, 1, 0, selgraf), NULL);
        else engineEmitEvent(EV_WORLDROTATE, engineNewWR(1, PI/2.0, 0, 1, 0, selgraf), NULL);
    }
    if (keysym.sym == 99 && type == K_KDOWN) { // C
        if (modctrl) engineEmitEvent(EV_WORLDROTATE, engineNewWR(0, -PI/4.0, 0, 1, 0, selgraf), NULL);
        else engineEmitEvent(EV_WORLDROTATE, engineNewWR(1, PI, 0, 1, 0, selgraf), NULL);
    }
    if (keysym.sym == 118 && type == K_KDOWN) { // V
        if (modctrl) engineEmitEvent(EV_WORLDROTATE, engineNewWR(0, -PI/8.0, 0, 1, 0, selgraf), NULL);
        else engineEmitEvent(EV_WORLDROTATE, engineNewWR(1, 3*PI/2.0, 0, 1, 0, selgraf), NULL);
    }
    
    /* SELETION */
    if (keysym.sym == 113 && type == K_KDOWN) { // Q
        if (modctrl) engineEmitEvent(EV_SELECTION, engineNewS(1, 0, 0, selgraf), NULL);
        else engineEmitEvent(EV_SELECTION, engineNewS(0, -1, 0, selgraf), NULL);
    }
    if (keysym.sym == 119 && type == K_KDOWN) { // W
        if (modctrl) engineEmitEvent(EV_SELECTION, engineNewS(2, 0, 0, selgraf), NULL);
        else engineEmitEvent(EV_SELECTION, engineNewS(0, 1, 0, selgraf), NULL);
    }
    if (keysym.sym == 101 && type == K_KDOWN) { // E
        if (modctrl) engineEmitEvent(EV_SELECTION, engineNewS(3, 0, 0, selgraf), NULL);
        else engineEmitEvent(EV_SELECTION, engineNewS(0, 0, -1, selgraf), NULL);
    }
    if (keysym.sym == 114 && type == K_KDOWN) { // R
        if (modctrl) engineEmitEvent(EV_SELECTION, engineNewS(4, 0, 0, selgraf), NULL);
        else engineEmitEvent(EV_SELECTION, engineNewS(0, 0, 1, selgraf), NULL);
    }

    /* GRAFVISIBILITY */
    if (keysym.sym == 49 && type == K_KDOWN && modctrl) { // 1
        selgraf = 0;
        if(modshift) engineEmitEvent(EV_GRAPHVISIBILITY, engineNewGV(0, selgraf), NULL);
        else  engineEmitEvent(EV_GRAPHVISIBILITY, engineNewGV(1, selgraf), NULL);
    }
    if (keysym.sym == 50 && type == K_KDOWN && modctrl) { // 2
        selgraf = 1;
        if(modshift) engineEmitEvent(EV_GRAPHVISIBILITY, engineNewGV(0, selgraf), NULL);
        else  engineEmitEvent(EV_GRAPHVISIBILITY, engineNewGV(1, selgraf), NULL);
    }
    if (keysym.sym == 51 && type == K_KDOWN && modctrl) { // 3
        selgraf = 2;
        if(modshift) engineEmitEvent(EV_GRAPHVISIBILITY, engineNewGV(0, selgraf), NULL);
        else  engineEmitEvent(EV_GRAPHVISIBILITY, engineNewGV(1, selgraf), NULL);
    }
    if (keysym.sym == 52 && type == K_KDOWN && modctrl) { // 4
        selgraf = 3;
        if(modshift) engineEmitEvent(EV_GRAPHVISIBILITY, engineNewGV(0, selgraf), NULL);
        else  engineEmitEvent(EV_GRAPHVISIBILITY, engineNewGV(1, selgraf), NULL);
    }

    /* GRAFSELECTION */
    if (keysym.sym == 49 && type == K_KDOWN && modalt) { // 1
        selgraf = 0;
    }
    if (keysym.sym == 50 && type == K_KDOWN && modalt) { // 2
        selgraf = 1;
    }
    if (keysym.sym == 51 && type == K_KDOWN && modalt) { // 3
        selgraf = 2;
    }
    if (keysym.sym == 52 && type == K_KDOWN && modalt) { // 4
        selgraf = 3;
    }

    /* NADIR/TRANSPARENT CHANGE */
    if (keysym.sym == 116 && type == K_KDOWN) { // T
        engineEmitEvent(EV_SELECTMODE, engineNewSM(1, selgraf), NULL);
    }
    if (keysym.sym == 121 && type == K_KDOWN) { // Y
        engineEmitEvent(EV_SELECTMODE, engineNewSM(0, selgraf), NULL);
    }
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * MOUSE * * * * * * * * * * * * * * * * * */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void mouseHandleEvent (SDL_Event event) {
    
    switch (event.type) {

        case SDL_MOUSEBUTTONDOWN:
            mouseHandleButton ( (SDL_MouseButtonEvent *) &event);
            break;

        case SDL_MOUSEMOTION:
            mouseHandleMotion ( (SDL_MouseMotionEvent *) &event);
            break;

        case SDL_MOUSEWHEEL:
            mouseHandleWheel ( (SDL_MouseWheelEvent *) &event);
            break;
    }

}

void mouseHandleButton (SDL_MouseButtonEvent *event) {}

void mouseHandleWheel (SDL_MouseWheelEvent *event) {
    engineEmitEvent(EV_CAMERAMOVE,
        engineNewCM(1, 0.0, 1 - event->y * M_DEFscrollspeed, 0, 0, 0),
        NULL);

    /* Legacy aproach */
    /*serverSendZoomEvent(1 - event->y * M_DEFscrollspeed);*/
}

void mouseHandleMotion (SDL_MouseMotionEvent *event) {
    /*if only the left button is pressed - relative camera move*/
    if( (event->state & 1 << SDL_BUTTON_LEFT-1  ) &&
      ! (event->state & 1 << SDL_BUTTON_MIDDLE-1) &&
      ! (event->state & 1 << SDL_BUTTON_RIGHT-1 ) ) {

        const Uint8* keys = SDL_GetKeyboardState (NULL);

        int width, height;
        engineGetWindowSize (&width, &height);

        double distance = 0.0;
        double angle = 0.0;
        double x, y = 0.0, z;

        if (keys[SDL_SCANCODE_X]) {
            x = 0.0;
            y = event->yrel;
            z = event->xrel;

            z /= (double)width;
            y /= (double)height;
        } else if (keys[SDL_SCANCODE_Z]) {
            x = event->xrel;
            y = event->yrel;
            z = 0.0;

            x /= (double)width;
            y /= (double)height;
        } else {
            x = event->xrel;
            y = 0.0;
            z = event->yrel;

            x /= (double)width;
            z /= (double)height;
        }

        x *= M_DEFrelativespeed;
        y *= M_DEFrelativespeed;
        z *= M_DEFrelativespeed;

        engineEmitEvent(EV_CAMERAMOVE,
            engineNewCM(0, angle, distance, x, y, z),
            NULL);

        /* Legacy aproach */
        /*serverSendCMEvent(0, angle, distance, x, y, z );*/
    }
    /*if only the right button is pressed - absolute camera move*/
    else if( (event->state & 1 << SDL_BUTTON_RIGHT-1 ) &&
           ! (event->state & 1 << SDL_BUTTON_LEFT-1  ) &&
           ! (event->state & 1 << SDL_BUTTON_MIDDLE-1) ) {

        const Uint8* keys = SDL_GetKeyboardState (NULL);

        int width, height;
        engineGetWindowSize (&width, &height);

        double distance = 0.0;
        double angle = 0.0;
        double x, y, z;

        if (keys[SDL_SCANCODE_X]) {
            x = 0.0;
            y = event->yrel;
            z = event->xrel;

            z = (event->x - width / 2.0) / width * M_DEFsphereradius * 2.0;
            y = (event->y - height / 2.0) / height * M_DEFsphereradius * 2.0;
        } else if (keys[SDL_SCANCODE_Z]) {
            x = event->xrel;
            y = event->yrel;
            z = 0.0;

            x = (event->x - width / 2.0) / width * M_DEFsphereradius * 2.0;
            y = (event->y - height / 2.0) / height * M_DEFsphereradius * 2.0;
        } else {
            x = event->xrel;
            y = 0.0;
            z = event->yrel;

            x = (event->x - width / 2.0) / width * M_DEFsphereradius * 2.0;
            z = (event->y - height / 2.0) / height * M_DEFsphereradius * 2.0;
        }

        engineEmitEvent(EV_CAMERAMOVE,
            engineNewCM(1, angle, distance, x, y, z),
            NULL);

        /* Legacy aproach */
        /*serverSendCMEvent (1, angle, distance, x, y, z );*/
    }
    /*if both left and right buttons are pressed*/
    else if( (event->state & 1 << SDL_BUTTON_MIDDLE-1) &&
           ! (event->state & 1 << SDL_BUTTON_LEFT-1  ) &&
           ! (event->state & 1 << SDL_BUTTON_RIGHT-1 ) ) {

        int width, height;
        engineGetWindowSize (&width, &height);

        double angle = -event->xrel;
        angle /= width;
        angle *= M_PI;

        engineEmitEvent(EV_CAMERAMOVE,
            engineNewCM(0, angle, 0.0, 0, 1, 0),
            NULL);

        /* Legacy aproach */
        /*serverSendYREvent (angle);*/
    }

}

