#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL_thread.h>
#include "engine.h"
#include "events.h"

#define S_DEFport 12345
#define S_DEFbuff 1024

typedef struct _serverconf{
    Uint16 port;
    UDPsocket socket;
    UDPpacket *in;
    SDL_Thread *thread;
    unsigned char stop;
    int exitstatus;
    SDLNet_SocketSet socketset;
}serverconf;

serverconf server;

void serverRun (int argc, char **argv);
void serverStop();

int serverMainLoop (void *data);
void serverParseData();

int serverInit (int argc, char **argv);
void serverLoadConfig (int argc, char **argv);

/*The following functions are defined for testing purposes*/
void serverSendCMEvent (int how, float angle, float distance, float x, float y, float z);
void serverSendCREvent (int how, float angle, float x, float y, float z);
void serverSendWREvent (int how, float angle, float x, float y, float z);
void serverSendYREvent (float angle);
void serverSendZoomEvent (float zoom);
void serverSendHSEvent(int direction);
void serverSendDSEvent(int direction);
