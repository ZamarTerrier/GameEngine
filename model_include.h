#ifndef MODEL_INCLUDE_H
#define MODEL_INCLUDE_H

#define MAX_BONES 64

#include "stdinclude.h"

typedef struct skin_vertex {
    uint8_t bone_index[4];
    uint8_t bone_weight[4];
} skin_vertex;

typedef struct engine_node {
    int32_t parent_index;

    mat4 geometry_to_node;
    mat4 node_to_parent;
    mat4 node_to_world;
    mat4 geometry_to_world;
    mat4 normal_to_world;
} engine_node;

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
} engine_node_anim;

typedef struct viewer_blend_channel_anim {
    float const_weight;
    float *weight;
} engine_blend_channel_anim;

typedef struct fbx_anim {
    const char *name;
    float time_begin;
    float time_end;
    float framerate;
    size_t num_frames;

    engine_node_anim *nodes;
    engine_blend_channel_anim *blend_channels;
} engine_anim;

typedef struct{

    Vertex3D *verts;
    uint32_t num_verts;

    uint32_t* indices;
    uint32_t num_indices;

} engine_mesh;

typedef struct viewer_blend_channel {
    float weight;
} engine_blend_channel;

#endif // MODEL_INCLUDE_H
