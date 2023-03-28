#ifndef QUAD_OBJECT_H
#define QUAD_OBJECT_H

#include "engine_includes.h"

#include "gameObject2D.h"

typedef struct{
    GameObject2D go;
} QuadObject;

void QuadObjectInit(QuadObject *qu);

void QuadObjectAddDefault(QuadObject *qu, void *render);

#endif // QUAD_OBJECT_H
