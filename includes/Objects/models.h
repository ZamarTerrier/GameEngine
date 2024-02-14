#ifndef MODELS_H
#define MODELS_H

#include "Variabels/engine_includes.h"
#include "Variabels/model_include.h"

#include "Objects/gameObject.h"

#include "Core/graphicsObject.h"
#include "Core/e_transform.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    GraphicsObject graphObj;
    GameObjectImage *diffuse;
    GameObjectImage *specular;
    GameObjectImage *normal;
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

void ModelModelBufferUpdate(ModelObject3D* mo, uint32_t indx_node, BluePrintDescriptor *descriptor);
void ModelLightMatrixUpdate(ModelObject3D* mo, uint32_t indx_node, BluePrintDescriptor *descriptor);
void ModelInvMatrixBuffer(ModelObject3D* mo, uint32_t indx_node, BluePrintDescriptor *descriptor);
void ModelDescriptorLightObjectsUpdate(ModelObject3D* mo, uint32_t indx_node, BluePrintDescriptor *descriptor);

void ModelDefaultDraw(ModelObject3D* mo, void *command);
void ModelDefaultUpdate(ModelObject3D* mo);
void ModelClean(ModelObject3D* mo);
void ModelRecreate(ModelObject3D* mo);
void ModelDestroy(ModelObject3D* mo);

void ModelSetLightEnable(void *obj, bool enable);
void ModelSetSelCameraEnable(void *obj, bool enable);
void ModelSetSomeViewport(ModelObject3D* mo, float x, float y, float height, float width);

void ModelDefaultInit(ModelStruct *model, DrawParam *dParam);

#ifdef __cplusplus
}
#endif

#endif // MODELS_H
