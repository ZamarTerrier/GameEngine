#ifndef GRASS_OBJECT_H
#define GRASS_OBJECT_H

#include "Variabels/engine_includes.h"

#include "gameObject3D.h"

typedef struct{
    GameObject3D go;
} GrassObject;

void GrassObjectSetDefaultDescriptor(GrassObject *grass, DrawParam *dParam);
void GrassObjectSetInstanceDescriptor(GrassObject *grass, DrawParam *dParam);

void GrassObjectInit(GrassObject *grass, DrawParam *dParam);

#endif // GRASS_OBJECT_H
