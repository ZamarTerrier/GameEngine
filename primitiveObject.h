#ifndef PRIMITIVEOBJECT_H
#define PRIMITIVEOBJECT_H

#include "gameObject3D.h"

enum PrimitiveObjectType{
    ENGINE_PRIMITIVE3D_LINE,
    ENGINE_PRIMITIVE3D_TRIANGLE,
    ENGINE_PRIMITIVE3D_QUAD,
    ENGINE_PRIMITIVE3D_PLANE,
    ENGINE_PRIMITIVE3D_CUBE,
    ENGINE_PRIMITIVE3D_CUBESPHERE,
    ENGINE_PRIMITIVE3D_ICOSPHERE,
    ENGINE_PRIMITIVE3D_SPHERE,
    ENGINE_PRIMITIVE3D_CAPSULE,
    ENGINE_PRIMITIVE3D_CONE,
    ENGINE_PRIMITIVE3D_CYLINDER,
    ENGINE_PRIMITIVE3D_SKYBOX,
};

typedef struct{
    GameObject3D go;
} PrimitiveObject;

typedef struct{
    float radius;
    int sectorCount;
    int stackCount;
} SphereParam;

typedef struct{
    float radius;
    int verperrow;
} CubeSphereParam;

typedef struct{
    int sectorCount;
    int stackCount;
} PlaneParam;

typedef struct{
    float height;
    int sectorCount;
    int stackCount;
} ConeParam;

void PrimitiveObjectInit(GameObject3D *go, vec3 size, vec3 position, DrawParam dParam, char type, void *params);

#endif // PRIMITIVEOBJECT_H
