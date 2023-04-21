#ifndef PRIMITIVEOBJECT_H
#define PRIMITIVEOBJECT_H

#include "engine_includes.h"

#include "gameObject3D.h"

#ifdef __cplusplus
extern "C"
{
#endif

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
    void *params;
    uint32_t type;
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
    uint32_t texture_width;
    uint32_t texture_height;
} PlaneParam;

typedef struct{
    float height;
    int sectorCount;
    int stackCount;
} ConeParam;


void PrimitiveObjectInit(PrimitiveObject *po, DrawParam *dParam, char type, void *params);
void PrimitiveObjectInitDefault(PrimitiveObject *po, DrawParam *dParam, char type, void *params);

void PrimitiveObjectAddDefault(PrimitiveObject *po, DrawParam *dParam);

void *PrimitiveObjectGetVertex(PrimitiveObject *po);
void PrimitiveObjectDiffuseTextureSetData(PrimitiveObject *po, void *data, uint32_t size_data, uint32_t offset);
void PrimitiveObjectDiffuseSetTexture(PrimitiveObject *po, const char *path);

#ifdef __cplusplus
}
#endif

#endif // PRIMITIVEOBJECT_H
