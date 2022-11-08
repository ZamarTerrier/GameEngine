#ifndef INTERSECTIONS3D_H
#define INTERSECTIONS3D_H

#include "stdinclude.h"

typedef struct{
    vec3 position;
    float height;
    float radius;
} InterCapsuleParam;

typedef struct{
    vec3 center;
    float radius;
} InterSphereParam;

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
    vec3 position;
    vec3 direction;
    float distance;
} InterRayParam;

int IntersectionSphereTriangle(vec3 sPos, float r, vec3 p0, vec3 p1, vec3 p2, vec3 *resPos, float *dist, float *depth, vec3 *dir);
int IntersectionCapsuleShape(void *obj1, void *shape, float *dist, float *depth, vec3 *dir);
int IntersectionSphereShape(void *obj1, void *shape, float *dist, float *depth, vec3 *dir);
int IntersectionSphereToSphere(void *obj1, void *obj2, float *dist, float *depth, vec3 *dir);
int IntersectionSphereCapsule(void *obj1, void *obj2, float *dist, float *depth, vec3 *dir);
int IntersectionCapsuleCapsule(void *obj1, void *obj2, float *dist, float *depth, vec3 *dir);
int IntersectionSphereAABB(void *obj1, void *obj2, float *dist, float *depth, vec3 *dir);
int IntersectionAABBPlane(void *obj, vec3 n, float d);
int IntersectionAABBAABB(void *obj1, void *obj2, float *dist, float *depth, vec3 *dir);
int IntersectionAABBCapsule(void *obj1, void *obj2, float *dist, float *depth, vec3 *dir);
int IntersectionTriangleAABB(vec3 v0, vec3 v1, vec3 v2, void *obj, float *dist, float *depth, vec3 *dir);

#endif // INTERSECTIONS3D_H
