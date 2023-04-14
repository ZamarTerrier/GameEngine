#ifndef QUAD_OBJECT_H
#define QUAD_OBJECT_H

#include "engine_includes.h"

#include "gameObject2D.h"

typedef enum{
    ENGINE_QUAD_TYPE_IMAGE,
    ENGINE_QUAD_TYPE_DEPTH,
    ENGINE_QUAD_TYPE_CUBE,
} EngineQuadType;

typedef struct{
    GameObject2D go;
    uint32_t type;
} QuadObject;

void QuadObjectInit(QuadObject *qu, uint32_t type);

void QuadObjectAddDefault(QuadObject *qu, void *render);

#endif // QUAD_OBJECT_H
