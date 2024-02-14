#ifndef WATER_OBJECT_H
#define WATER_OBJECT_H

#include "Variabels/engine_includes.h"

#include "gameObject3D.h"

typedef struct{
    float time;
} WaterBuffer;

typedef struct{
    GameObject3D go;
} WaterObject;

void WaterObjectSetDefaultDescriptor(WaterObject *water, DrawParam *dParam);

void WaterObjectInit(WaterObject *water, DrawParam *dParam, uint32_t size);
void WaterObjectInitDefault(WaterObject *water, DrawParam *dParam, uint32_t size);

#endif // WATER_OBJECT_H
