#ifndef MODELS_H
#define MODELS_H

#include "stdinclude.h"
#include "model_include.h"

#include "gameObject3D.h"

#include "glTFLoader.h"

typedef struct{

    float anim_time;

    engine_node *nodes;
    uint32_t num_nodes;

    engine_mesh *meshes;
    uint32_t num_meshes;

    engine_anim *animations;
    uint32_t num_animations;

    engine_blend_channel *blend_channels;
    size_t num_blend_channels;
} FBXStruct;

typedef struct{
    GraphicsObject3D graphObj;
    Transform3D transform;
    uint32_t id_node;
} ModelStruct;

typedef struct{
    GameObject self;
    void *obj;
    glTFStruct *glTF;
    FBXStruct *fbx;
    ModelStruct *models;
    uint32_t num_models;
} ModelObject3D;

void ModelDefaultInit(ModelStruct *model, DrawParam dParam);
void ModelDefaultDraw(ModelObject3D* mo);
void ModelDefaultUpdate(ModelObject3D* mo);
void ModelClean(ModelObject3D* mo);
void ModelRecreate(ModelObject3D* mo);
void ModelDestroy(ModelObject3D* mo);

void Load3DObjModel(ModelObject3D * mo, char *filepath, DrawParam dParam);
void Load3DFBXModel(ModelObject3D * mo, char *filepath, DrawParam dParam);

#endif // MODELS_H
