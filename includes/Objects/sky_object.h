#ifndef SKY_OBJECT_H
#define SKY_OBJECT_H

#include "Variabels/engine_includes.h"

#include "gameObject2D.h"

typedef enum{
    ENGINE_SKY_TYPE_NIGHT,
    ENGINE_SKY_TYPE_DAY,
    ENGINE_SKY_TYPE_ATMOSPHERIC
} EngineSkyType;

typedef struct{
    float time;
    vec3 cam_pos __attribute__ ((aligned (16)));
    vec3 cam_rot __attribute__ ((aligned (16)));
    vec3 dir_light __attribute__ ((aligned (16)));
} SkyBuffer;

typedef struct{
    GameObject2D go;
    uint32_t type;
} SkyObject;

void SkyObjectInit(SkyObject *so, DrawParam *dParam, EngineSkyType type);
void SkyObjectSetMousePos(SkyObject *so, float pos_x, float pos_y);

void SkyObjectAddDefault(SkyObject *so, void *render);

#endif // SKY_OBJECT_H
