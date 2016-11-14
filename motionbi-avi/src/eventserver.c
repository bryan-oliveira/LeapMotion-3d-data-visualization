#include "eventserver.h"

/*Calls the initialization function and creates a thread where the server
  will run.*/
void serverRun (int argc, char **argv) {
    int error;
    if (error = serverInit(argc, argv))
        exit(error);
    server.thread = SDL_CreateThread( serverMainLoop, "server", NULL );
}

/*Sets up a flag responsible for stopping the server's mainloop. Then the
  calling thread is blocked waiting for the server thread to finish.*/
void serverStop() {
    server.stop = 1;
    SDL_WaitThread(server.thread, &server.exitstatus);
}

/*Loads the server's configuration. --port <port number> or -p <port number>
  can be used in the command line to change the default port value (12345).*/
void serverLoadConfig (int argc, char **argv) {
    /*set default values*/
    server.port = S_DEFport;

    /*override defaults with values set from command line*/
    int i;
    for (i = 1; i < argc; i++) {
        if (strcmp (argv[i], "--port") == 0 || strcmp (argv[i], "-p") == 0 ) {
            server.port = (Uint16)atoi(argv[++i]);
        }
    }
}

/*Initializes everything needed by the server. Initializes SDL modules, loads 
  the server's configuration, creates the server's socket and allocates a packet
  for socket input reading.*/
int serverInit (int argc, char **argv) {

    if (SDLNet_Init() == -1) {
        printf ("SDLNet_Init: %s\n", SDLNet_GetError() );
        return 2;
    }

    serverLoadConfig(argc, argv);

    if (! (server.socket = SDLNet_UDP_Open (server.port) ) ){
        printf ("SDLNet_UDP_Open: %s\n", SDLNet_GetError() );
        return 3;
    }

    if (! (server.in = SDLNet_AllocPacket (S_DEFbuff) ) ) {
        printf ("SDLNet_AllocPacket: %s\n", SDLNet_GetError() );
        return 5;
    }
    
    if (!(server.socketset = SDLNet_AllocSocketSet(1))) {
        printf("SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
        return 10;
    }
    
    if (SDLNet_UDP_AddSocket(server.socketset, server.socket) == -1) {
        printf("SDLNet_AddSocket: %s\n", SDLNet_GetError());
        return 11;
    }
    
    server.stop = 0;
    
    return 0;
}

/*This function implements the read, parse, trigger loop untill a stop flag is
  set. The function reads input from the socket, parses it and then triggers an
  event at the engine.*/
int serverMainLoop(void *data) {

    printf("INFO: Event server running on port %d\n", server.port);

    while (!server.stop) {
        // Waits for packets in maximum intervals of 200 ms
        int ready = SDLNet_CheckSockets(server.socketset, 200);
        if (ready == -1)
            printf("SDLNet_CheckSockets: %s\n", SDLNet_GetError());
        if (ready <= 0)
            continue;
        
        // Get a new packet from UDP socket
        int recvstatus = SDLNet_UDP_Recv(server.socket, server.in);
        if (recvstatus == -1)
            printf ("SDLNet_UDP_Recv: %s\n", SDLNet_GetError() );
        if (recvstatus == 1)
            serverParseData();
    }

    SDLNet_UDP_Close(server.socket);
    SDLNet_FreePacket(server.in);

    return 0;
}

/*This function parses the data received through the server's socket and
  triggers the events described in the read data.*/
void serverParseData() {
#ifdef DEBUG
    printf ("UDP packet: len %d\n", server.in->len);
#endif
    event * ev = (event *)server.in->data;
    evdata * data = malloc(sizeof(evdata));
    memcpy(data, &ev->data, sizeof(evdata));
    engineEmitEvent(ev->type, data, NULL);
}


/**********************************************************
 *The following functions are defined for testing purposes*
 **********************************************************/

void serverSendEvent(evtype type, const evdata* data) {
    UDPsocket outsocket = SDLNet_UDP_Open(0);
    UDPpacket *pkt = SDLNet_AllocPacket(sizeof(event));
    pkt->len = sizeof(event);
    SDLNet_ResolveHost(&pkt->address, "127.0.0.1", server.port);
    
    event *ev = (event*) pkt->data;
    ev->type = type;
    ev->data = *data;
    
    SDLNet_UDP_Send(outsocket, -1, pkt);
    SDLNet_UDP_Close(outsocket);
    SDLNet_FreePacket(pkt);
}

/*Sends a message with a Camera Move Event to this same server.*/
void serverSendCMEvent(int how, float angle, float distance, float x, float y, float z) {
    evdata ed = {.cameramove = {
        .how = how,
        .angle = angle,
        .distance = distance,
        .x = x,
        .y = y,
        .z = z,
    }};
    serverSendEvent(EV_CAMERAMOVE, &ed);
}

/*Sends a message with a World Rotate Event to this same server.*/
void serverSendWREvent(int how, float angle, float x, float y, float z) {
    evdata ed = {.worldrotate = {
        .how = how,
        .angle = angle,
        .x = x,
        .y = y,
        .z = z,
    }};
    serverSendEvent(EV_WORLDROTATE, &ed);
}

