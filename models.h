#ifndef MODELS_H
#define MODELS_H

#include "tinyobj_loader.h"

#include "stdinclude.h"
#include "gameObject3D.h"

typedef struct{
    GameObject3D go;
    tinyobj_attrib_t attrib;
    tinyobj_shape_t* shapes;
    tinyobj_material_t* materials;
    uint64_t num_shapes;
    uint64_t num_materials;
    char* warn;
    char* err;
} ModelObject3D;

void Load3DObjModel(ModelObject3D * mo, char *filepath, DrawParam dParam);
void DestroyOBJModel(ModelObject3D *model);

#endif // MODELS_H
