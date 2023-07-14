#ifndef SHAPEOBJECT_H
#define SHAPEOBJECT_H

#include "gameObject2D.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    vec2 position;
    vec2 direction;
    float length;
    vec3 color;
} LineParams;

typedef struct{
    float size;
    vec3 color;
} QuadParams;

typedef struct{
    float radius;
    int freq;
    vec3 color;
} CircleParams;

typedef struct{
    vec2 *positions;
    uint32_t num_positions;
    vec3 color;
} ShapeParams;

typedef enum{
    ENGINE_SHAPE_OBJECT_LINE,
    ENGINE_SHAPE_OBJECT_QUAD,
    ENGINE_SHAPE_OBJECT_CIRCLE,
    ENGINE_SHAPE_OBJECT_SHAPE
} ShapeType;

typedef struct{
    GameObject2D go;
    uint32_t type;
} ShapeObject;

void ShapeObjectInit(ShapeObject *so, DrawParam *dParam, ShapeType type, void *param);

void ShapeObjectInitDefault(ShapeObject *so, DrawParam *dParam, ShapeType type, void *param);

void ShapeObjectAddDefault(ShapeObject *so, void *render);
void ShapeObjectAddShaderBuilder(ShapeObject *so, void *render, void *vertex, void *fragment);

#ifdef __cplusplus
}
#endif

#endif // SHAPEOBJECT_H
