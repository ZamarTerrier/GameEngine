#ifndef TREE_OBJECT_H
#define TREE_OBJECT_H

#include "engine_includes.h"

#include "gameObject3D.h"

typedef enum{
    ENGINE_TREE_OBJECT_TYPE_VERTEX,
    ENGINE_TREE_OBJECT_TYPE_SDF,
} TreeObjectType;

typedef struct{
    uint32_t height;
    vec3 start_rotation;
    vec3 mutable_rotation;
    float radius;
    float grad;
    uint32_t max_branches;
    vec3 tree_color;
    vec3 leaf_color;
} TreeParams;

typedef struct{
    vec3 *positions;
    uint32_t num_positions;
    uint32_t *indexes;
    uint32_t num_indexes;
} TreeVerts;

typedef struct{
    GameObject3D go;
} TreeObject;

void TreeObjectInitInstances(GameObject3D *go);
void TreeObjectInit(TreeObject *to, uint32_t type, DrawParam *dParam, void *arg);
void TreeObjectInitDefault(TreeObject *to, uint32_t type, DrawParam *dParam, void *arg);
void TreeObjectSetInstanceDescriptor(TreeObject *to, DrawParam *dParam);

#endif // TREE_OBJECT_H
