#pragma once

#include "Variabels/engine_includes.h"

#include "Objects/gameObject3D.h"
#include "Objects/models.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct viewer_node_anim {
    float time_begin;
    float framerate;
    size_t num_frames;
    vec4 const_rot;
    vec3 const_pos;
    vec3 const_scale;
    vec4 *rot;
    vec3 *pos;
    vec3 *scale;
} engine_fbx_node_anim;

typedef struct viewer_blend_channel_anim {
    float const_weight;
    float *weight;
} engine_fbx_blend_channel_anim;

typedef struct fbx_anim {
    const char *name;
    float time_begin;
    float time_end;
    float framerate;
    size_t num_frames;

    engine_fbx_node_anim *nodes;
    engine_fbx_blend_channel_anim *blend_channels;
} engine_fbx_anim;


typedef struct{
    int32_t parent_index;

    mat4 geometry_to_node;
    mat4 node_to_parent;
    mat4 node_to_world;
    mat4 geometry_to_world;
    mat4 normal_to_world;
} engine_fbx_node;

typedef struct viewer_blend_channel {
    float weight;
} engine_fbx_blend_channel;

typedef struct{

    float anim_time;

    engine_fbx_node *nodes;
    uint32_t num_nodes;

    engine_model_mesh *meshes;
    uint32_t num_meshes;

    engine_fbx_anim *animations;
    uint32_t num_animations;

    engine_fbx_blend_channel *blend_channels;
    size_t num_blend_channels;
} FBXStruct;

void Load3DFBXNextFrame(ModelObject3D *mo);
void Load3DFBXModel(ModelObject3D * mo, char *filepath, DrawParam *dParam);

#ifdef __cplusplus
}
#endif
