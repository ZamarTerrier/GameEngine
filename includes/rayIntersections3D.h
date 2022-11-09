#ifndef RAYINTERSECTIONS3D_H
#define RAYINTERSECTIONS3D_H

#include "std_intersect.h"

typedef struct{
    vec3 position;
    vec3 direction;
    float distance;
} InterRayParam;

int Intersect3DRaySphere(IntRayParam *param, vec3 ObjP, float *t, vec3 *q);

// Intersect segment S(t)=sa+t(sb-sa), 0<=t<=1 against cylinder specified by p, q and r
int Intersect3DSegmentCylinder(IntRayParam *param, vec3 p, vec3 q, float r, float *t, vec3 *q_res);

int Intersect3DayAABB(IntRayParam *param,  vec3 obj_min, vec3 obj_max, float *tmin, float *tmax, vec3 *q);

float Intersect3DRayTriangle(void* shape, IntRayParam *param, vec3 *q);
#endif // RAYINTERSECTIONS3D_H
