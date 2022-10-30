#ifndef GLTFLOADER_H
#define GLTFLOADER_H

#include "stdinclude.h"

#include "model_include.h"

typedef struct{
    float time;
    vec3 vector3;
    vec4 vector4;
} anim_keyframe_struct;

typedef struct{
    anim_keyframe_struct *keyframes;
    uint32_t node_id;
    char type;
    int num_keyframes;
} anim_channel;

typedef struct{
    anim_channel *channels;
    int num_channels;
    int curr_key;
} anim_array;

typedef struct{
    int id_node;
    engine_mesh *mesh;
    char isModel;
    mat4 local_matrix;
    mat4 global_matrix;
} gltf_node;

typedef struct{
    anim_array *animations;
    uint32_t num_anims;
    float anim_time;
    gltf_node *nodes;
    uint32_t num_nodes;
    uint32_t num_meshes;
} glTFStruct;

void Load3DglTFModel(void *ptr, char *ascii, char *binary, DrawParam dParam);

#endif // GLTFLOADER_H
