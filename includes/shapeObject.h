#ifndef SHAPEOBJECT_H
#define SHAPEOBJECT_H

#include "gameObject2D.h"

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
    ENGINE_SHAPE_OBJECT_QUAD,
    ENGINE_SHAPE_OBJECT_CIRCLE,
    ENGINE_SHAPE_OBJECT_SHAPE
} ShapeType;

typedef struct{
    GameObject2D go;
} ShapeObject;

void ShapeObjectInit(ShapeObject *so, DrawParam dParam, ShapeType type, void *param);

#endif // SHAPEOBJECT_H
