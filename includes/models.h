#ifndef MODELS_H
#define MODELS_H

#include "engine_includes.h"

#include "model_include.h"

#include "gameObject.h"
#include "graphicsObject.h"
#include "transform.h"

typedef struct{
    GraphicsObject graphObj;    
    GameObjectImage *diffuse;
    GameObjectImage *specular;
    GameObjectImage *normal;
    int light_enable;
    bool selfCamera;
} ModelStruct;

typedef struct{
    ModelStruct *models;
    uint32_t num_models;
    uint32_t id_node;
    mat4 model;
} ModelNode;

typedef struct{
    GameObject self;
    Transform3D transform;

    ModelNode *nodes;
    uint32_t num_draw_nodes;

    mat4 join_mats[MAX_BONES];

    void *obj;
    uint32_t obj_size;
} ModelObject3D;

EIVertexInputBindingDescription ModelObject3DGetBindingDescription();

void ModelDefaultDraw(ModelObject3D* mo);
void ModelDefaultUpdate(ModelObject3D* mo);
void ModelClean(ModelObject3D* mo);
void ModelRecreate(ModelObject3D* mo);
void ModelDestroy(ModelObject3D* mo);

void ModelSetLightEnable(void *obj, bool enable);
void ModelSetSelCameraEnable(void *obj, bool enable);
void ModelSetSomeViewport(ModelObject3D* mo, float x, float y, float height, float width);

void ModelDefaultInit(ModelStruct *model, DrawParam *dParam);

#endif // MODELS_H
