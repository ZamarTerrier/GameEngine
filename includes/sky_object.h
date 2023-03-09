#ifndef SKY_OBJECT_H
#define SKY_OBJECT_H

#include "engine_includes.h"

#include "gameObject2D.h"

typedef struct{
    float time;
    vec3 cam_pos __attribute__ ((aligned (16)));
    vec3 cam_rot __attribute__ ((aligned (16)));
} SkyBuffer;

typedef struct{
    GameObject2D go;
    float mouse_x;
    float mouse_y;
} SkyObject;

void SkyObjectInit(SkyObject *so, DrawParam *dParam);
void SkyObjectSetMousePos(SkyObject *so, float pos_x, float pos_y);

#endif // SKY_OBJECT_H
