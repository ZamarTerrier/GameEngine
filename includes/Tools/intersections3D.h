#ifndef INTERSECTIONS3D_H
#define INTERSECTIONS3D_H

#include "Variabels/engine_includes.h"
#include "Variabels/std_intersect.h"

#ifdef __cplusplus
extern "C"
{
#endif

int IntersectionSpherePlane(void *plane, void *sphere, float *distance, float *depth, vec3 *dir);
int IntersectionSphereTerrain(void *terrain, void *sphere, float *distance, float *depth, vec3 *dir);
int IntersectionSphereTriangle(vec3 sPos, float r, vec3 p0, vec3 p1, vec3 p2, vec3 *resPos, float *dist, float *depth, vec3 *dir);
int IntersectionCapsuleShape(void *obj1, void *shape, float *dist, float *depth, vec3 *dir);
int IntersectionSphereShape(void *obj1, void *shape, float *dist, float *depth, vec3 *dir);
int IntersectionSphereToSphere(void *obj1, void *obj2, float *dist, float *depth, vec3 *dir);
int IntersectionSphereCapsule(void *obj1, void *obj2, float *dist, float *depth, vec3 *dir);
int IntersectionCapsuleCapsule(void *obj1, void *obj2, float *dist, float *depth, vec3 *dir);
int IntersectionSphereAABB(void *obj1, void *obj2, float *dist, float *depth, vec3 *dir);
int IntersectionAABBPlane(void *obj1, void *obj2, float *dist, float *depth, vec3 *dir);
int IntersectionAABBAABB(void *obj1, void *obj2, float *dist, float *depth, vec3 *dir);
int IntersectionAABBCapsule(void *obj1, void *obj2, float *dist, float *depth, vec3 *dir);
int IntersectionTriangleAABB(vec3 v0, vec3 v1, vec3 v2, void *obj, float *dist, float *depth, vec3 *dir);

#ifdef __cplusplus
}
#endif

#endif // INTERSECTIONS3D_H
