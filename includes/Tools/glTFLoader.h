#ifndef GLTFLOADER_H
#define GLTFLOADER_H

#include "Variabels/engine_includes.h"
#include "Variabels/model_include.h"

#ifdef __cplusplus
extern "C"
{
#endif

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
    char name[256];
    int id_node;
    int id_parent;
    char isModel;
    engine_model_mesh **mesh;
    uint32_t num_mesh;
    mat4 global_matrix;
    mat4 local_matrix;
} engine_gltf_node;

typedef struct{
    float anim_time;

    engine_gltf_anim *animations;
    uint32_t num_anims;

    join_mat_struct *joint_mats;
    uint32_t num_join_mats;

    engine_gltf_node *nodes;
    uint32_t num_nodes;

    uint32_t num_meshes;

    char *name;
    char *path;
} glTFStruct;

void Load3DglTFNextFrame(void *ptr, float time, int num_animation);
void Load3DglTFModel(void *ptr, char *path, char *name, uint8_t type, DrawParam *dParam);

#ifdef __cplusplus
}
#endif

#endif // GLTFLOADER_H
