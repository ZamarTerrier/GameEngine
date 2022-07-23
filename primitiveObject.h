#ifndef PRIMITIVEOBJECT_H
#define PRIMITIVEOBJECT_H

#include "gameObject3D.h"

enum PrimitiveObjectType{
    ENGINE_PRIMITIVE3D_TRIANGLE,
    ENGINE_PRIMITIVE3D_QUAD,
    ENGINE_PRIMITIVE3D_PLANE,
    ENGINE_PRIMITIVE3D_CUBE,
    ENGINE_PRIMITIVE3D_SPHERE,
    ENGINE_PRIMITIVE3D_CAPSULE,
    ENGINE_PRIMITIVE3D_CONE,
    ENGINE_PRIMITIVE3D_CYLINDER
};

typedef struct{
    GameObject3D go;
} PrimitiveObject;

void PrimitiveObjectInit(GameObject3D *go, vec3 size, vec3 position, const char* texturePath, const char* vertShader, const char* fragShader, char type);

#endif // PRIMITIVEOBJECT_H
