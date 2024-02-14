#ifndef MODEL_INCLUDE_H
#define MODEL_INCLUDE_H

#define MAX_BONES 64

#include "engine_includes.h"

typedef struct{
    mat4 inv_mat;
    mat4 join_mat;
    uint32_t id_node;
} join_mat_struct;

typedef struct skin_vertex {
    uint8_t bone_index[4];
    uint8_t bone_weight[4];
} skin_vertex;

typedef struct{
    ModelVertex3D *verts;
    uint32_t num_verts;

    uint32_t* indices;
    uint32_t num_indices;

    int32_t *instance_node_indices;
    size_t num_instances;

    GameObjectImage *image;
    GameObjectImage *specular;
    GameObjectImage *normal;
} engine_model_mesh;

#endif // MODEL_INCLUDE_H
