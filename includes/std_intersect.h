#ifndef STD_INTERSECT_H
#define STD_INTERSECT_H

#include "engine_includes.h"

#include "e_math.h"

typedef struct{
    vec3 position;
    float radius;
    float height;
} InterCapsuleParam;

typedef struct{
    vec2 center;
    float radius;
} InterCircleParam;

typedef struct{
    vec3 center;
    float radius;
} InterSphereParam;


typedef struct{
    vec2 position;
    float size;
} InterSquareParam;

typedef struct{
    vec3 position;
    float size;
} InterAABBParam;

typedef struct {
    vec3 center; // OBB center point
    vec3 axes; // Local x-, y-, and z-axes
    vec3 halfwidth; // Positive halfwidth extents of OBB along each axis
} InterOBBParam;

typedef struct{
    vec2 position;
    vec2 direction;
    float distance;
} InterRay2DParam;

typedef struct{
    vec3 position;
    vec3 direction;
    float distance;
} InterRay3DParam;

typedef struct{
    vec2 p1;
    vec2 p2;
    vec2 p3;
} InterTriangleParam;

#endif // STD_INTERSECT_H
