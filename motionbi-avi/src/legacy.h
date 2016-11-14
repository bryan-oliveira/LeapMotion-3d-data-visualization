#pragma once

#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include "eventserver.h"
#include "engine.h"

#define M_DEFsphereradius 25.0
#define M_DEFrelativespeed 10.0
#define M_DEFscrollspeed 0.1
#define K_KDOWN 1
#define K_KUP 0


typedef struct _mousestate {
    double x, y;
    char left_btn;
    char right_btn;
    char middle_btn;
}mousestate;

mousestate mouse;


void keyboardEvtKeyboard(int type, SDL_Keysym keysym);


void mouseHandleEvent(SDL_Event event);

void mouseHandleButton (SDL_MouseButtonEvent *event);
void mouseHandleWheel (SDL_MouseWheelEvent *event);
void mouseHandleMotion (SDL_MouseMotionEvent *event);
