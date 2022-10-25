#ifndef MODELS_H
#define MODELS_H

#include "tinyobj_loader.h"

#include "stdinclude.h"
#include "gameObject3D.h"

#define MAX_BONES 64

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

    bool skinned;
    size_t num_bones;
    int32_t bone_indices[MAX_BONES];
    mat4 bone_matrices[MAX_BONES];

    int32_t *instance_node_indices;
    size_t num_instances;
} engine_mesh;

typedef struct viewer_blend_channel {
    float weight;
} engine_blend_channel;

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
    tinyobj_attrib_t attrib;
    tinyobj_shape_t* shapes;
    tinyobj_material_t* materials;
    uint64_t num_shapes;
    uint64_t num_materials;
    char* warn;
    char* err;
} OBJStruct;

typedef struct{
    GraphicsObject3D graphObj;
    Transform3D transform;
} ModelStruct;

typedef struct{
    GameObject self;
    OBJStruct *obj;
    FBXStruct *fbx;
    ModelStruct *models;
    uint32_t num_models;
} ModelObject3D;

void Load3DObjModel(ModelObject3D * mo, char *filepath, DrawParam dParam);
void Load3DFBXModel(ModelObject3D * mo, char *filepath, DrawParam dParam);

void DestroyFBXModel(ModelObject3D *model);
void DestroyOBJModel(ModelObject3D *model);

#endif // MODELS_H
