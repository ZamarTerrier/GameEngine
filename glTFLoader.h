#ifndef GLTFLOADER_H
#define GLTFLOADER_H

#include "stdinclude.h"

#include "model_include.h"

typedef struct{
    float time;
    vec3 vector3;
    vec4 vector4;
} engine_gltf_anim_keyframe;

typedef struct{
    engine_gltf_anim_keyframe *keyframes;
    uint32_t node_id;
    float min_time;
    float max_time;
    char type;
    int num_keyframes;
} engine_gltf_anim_channel;

typedef struct{
    engine_gltf_anim_channel *channels;
    int num_channels;
    int curr_key;
} engine_gltf_anim;

typedef struct{
    char *name;
    int id_node;
    int id_parent;
    engine_mesh *mesh;
    char isModel;
    mat4 global_matrix;
    mat4 local_matrix;
} engine_gltf_node;

typedef struct{
    float anim_time;

    engine_gltf_anim *animations;
    uint32_t num_anims;


    engine_gltf_node *nodes;
    uint32_t num_nodes;

    uint32_t num_meshes;
} glTFStruct;

void Load3DglTFNextFrame(void *ptr, float time);
void Load3DglTFModel(void *ptr, char *ascii, char *binary, DrawParam dParam);

#endif // GLTFLOADER_H