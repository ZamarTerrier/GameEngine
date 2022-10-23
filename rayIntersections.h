#ifndef RAYINTERSECTIONS_H
#define RAYINTERSECTIONS_H

#include "std_intersect.h"

int IntersectRaySphere(IntRayParam *param, vec3 ObjP, float *t, vec3 *q);

// Intersect segment S(t)=sa+t(sb-sa), 0<=t<=1 against cylinder specified by p, q and r
int IntersectSegmentCylinder(IntRayParam *param, vec3 p, vec3 q, float r, float *t, vec3 *q_res);

int IntersectRayAABB(IntRayParam *param,  vec3 obj_min, vec3 obj_max, float *tmin, float *tmax, vec3 *q);

float IntersectRayTriangle(void* shape, IntRayParam *param, vec3 *q);
#endif // RAYINTERSECTIONS_H
