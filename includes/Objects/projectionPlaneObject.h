#ifndef PROJECTIONPLANEOBJECT_H
#define PROJECTIONPLANEOBJECT_H

#include "Variabels/engine_includes.h"

#include "gameObject2D.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    GameObject2D go;
} ProjectionPlaneObject;

typedef struct{
    float time __attribute__ ((aligned (16)));
    vec3 camRot __attribute__ ((aligned (16)));
    vec3 camPos __attribute__ ((aligned (16)));
} ProjDataBuffer;

void ProjectionPlaneInit(GameObject2D *go, DrawParam dParam);

#ifdef __cplusplus
}
#endif

#endif // PROJECTIONPLANEOBJECT_H
