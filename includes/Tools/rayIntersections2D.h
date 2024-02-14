#ifndef RAYINTERSECTIONS2D_H
#define RAYINTERSECTIONS2D_H

#include "Variabels/std_intersect.h"

#ifdef __cplusplus
extern "C"
{
#endif

int IntersectRayCircle(InterRay2DParam *ray, InterCircleParam *circle, float *t, vec2 *q);
int IntersectRaySquare(InterRay2DParam *ray,  InterSquareParam *square, float *tmin, float *tmax, vec2 *q);
int IntersectRayShape( InterRay2DParam *ray, void *obj);

#ifdef __cplusplus
}
#endif

#endif // RAYINTERSECTIONS2D_H
