#ifndef MODELS_H
#define MODELS_H

#include "stdinclude.h"

#include "model_include.h"

#include "gameObject.h"
#include "graphicsObject.h"
#include "transform.h"

typedef struct{
    GraphicsObject3D graphObj;
    Transform3D transform;
    engine_mesh *mesh;
    uint32_t id_node;
} ModelStruct;

typedef struct{
    GameObject self;
    void *obj;
    uint32_t obj_size;
    ModelStruct *models;
    uint32_t num_models;
} ModelObject3D;

void ModelDefaultDraw(ModelObject3D* mo);
void ModelDefaultUpdate(ModelObject3D* mo);
void ModelClean(ModelObject3D* mo);
void ModelRecreate(ModelObject3D* mo);
void ModelDestroy(ModelObject3D* mo);

void* ModelDefaultInit(ModelStruct *model, DrawParam dParam, void* stbi_point);

#endif // MODELS_H
