#ifndef TREE_OBJECT_H
#define TREE_OBJECT_H

#include "engine_includes.h"

#include "gameObject3D.h"

typedef struct{
    vec3 *positions;
    uint32_t num_positions;
    uint32_t *indexes;
    uint32_t num_indexes;
} TreeVerts;

typedef struct{
    GameObject3D go;
} TreeObject;

void TreeObjectInit(TreeObject *to, DrawParam *dParam);
void TreeObjectInitDefault(TreeObject *to, DrawParam *dParam);

#endif // TREE_OBJECT_H
