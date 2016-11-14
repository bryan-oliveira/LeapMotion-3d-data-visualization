#pragma once

#include <stdint.h>

typedef enum _evtype {
    /* control events */
    EV_CAMERAMOVE     = 1,
    EV_SELECTION      = 5,
    EV_WORLDROTATE    = 6,
    EV_GRAPHVISIBILITY= 7,

    /* config events */
    EV_SELECTMODE     = 51,

    /* render events */
    EV_RENDER         = 100,
    EV_UPDATE         = 101,
} evtype;

/* Disable structure padding */
#pragma pack(1)

typedef struct _ev_cameramove {
    int32_t how;
    float distance;
    float angle;
    float x, y, z;
} ev_cameramove;

typedef struct _ev_selection {
    int32_t mode;
    int32_t hdir;
    int32_t ddir;
    int32_t graf;
} ev_selection;

typedef struct _ev_worldrotate {
    int32_t how;
    float angle;
    float x, y, z;
    int32_t graf;
} ev_worldrotate;

typedef struct _ev_selectmode {
    int32_t transparent; /* 1 to transparent, 0 to nadir */
    int32_t graf;
} ev_selectmode;

typedef struct _ev_graphvisibility {
    int32_t visibility; /*1 to show, 0 to hide*/
    int32_t graf;
} ev_graphvisibility;

/* Restore structure packing rules */
#pragma pack()

typedef union _evdata {
    ev_cameramove       cameramove;
    ev_selection        selection;
    ev_worldrotate      worldrotate;
    ev_selectmode       selectmode;
    ev_graphvisibility  graphvisibility;
} evdata;

typedef struct _event {
    evtype type;
    evdata data;
} event;

