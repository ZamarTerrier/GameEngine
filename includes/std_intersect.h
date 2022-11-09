#ifndef STD_INTERSECT_H
#define STD_INTERSECT_H

#include "stdinclude.h"

#include "e_math.h"

typedef struct{
    vec3 position;
    float radius;
    float height;
} IntCapsuleParam;

typedef struct{
    vec3 center;
    float radius;
} IntSphereParam;

typedef struct{
    vec3 position;
    float size;
} IntAABBParam;

typedef struct {
    vec3 center; // OBB center point
    vec3 axes; // Local x-, y-, and z-axes
    vec3 halfwidth; // Positive halfwidth extents of OBB along each axis
} IntOBBParam;

typedef struct{
    vec3 position;
    vec3 direction;
    float distance;
} IntRayParam;

#endif // STD_INTERSECT_H
