#ifndef RAYINTERSECTIONS2D_H
#define RAYINTERSECTIONS2D_H

#include "std_intersect.h"

int IntersectRayCircle(InterRay2DParam *ray, InterCircleParam *circle, float *t, vec2 *q);
int IntersectRaySquare(InterRay2DParam *ray,  InterSquareParam *square, float *tmin, float *tmax, vec2 *q);

#endif // RAYINTERSECTIONS2D_H
