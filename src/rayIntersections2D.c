#include "rayIntersections2D.h"

#include "gameObject2D.h"

int IntersectRayCircle(InterRay2DParam *ray, InterCircleParam *circle, float *t, vec2 *q)
{
    vec2 m = v2_sub(ray->position, circle->center);
    float b = v2_dot(m, ray->direction);
    float c = v2_dot(m, m) - circle->radius * circle->radius;

    // Exit if r’s origin outside s (c > 0) and r pointing away from s (b > 0)
    if (c > 0.0f && b > 0.0f)
        return 0;

    float discr = b*b - c;

    // A negative discriminant corresponds to ray missing sphere
    if (discr < 0.0f)
        return 0;

    // Ray now found to intersect sphere, compute smallest t value of intersection
    *t = -b - sqrt(discr);

    // If t is negative, ray started inside sphere so clamp t to zero
    if (*t < 0.0f)
        *t = 0.0f;

    if(*t > ray->distance)
        return 0;

    *q = v2_add(ray->position, v2_muls(ray->direction,*t ));

    return 1;
}

int IntersectRaySquare(InterRay2DParam *ray,  InterSquareParam *square, float *tmin, float *tmax, vec2 *q){

    *tmin = 0.0f;
    *tmax = 20000;

    vec2 obj_max = {square->position.x + square->size, square->position.y + square->size};
    vec2 obj_min = {square->position.x - square->size, square->position.y - square->size};

    float *d = (float *)&ray->direction;
    float *p = (float *)&ray->position;
    float *min = (float *)&obj_min;
    float *max = (float *)&obj_max;

    for(int i=0;i < 2;i++){
        float ood = 1.0f  / d[i];
        float t1 = (min[i] - p[i]) * ood;
        float t2 = (max[i] - p[i]) * ood;

        if(t1 > t2)
        {
            float t = t2;
            t2= t1;
            t1 = t;
        }

        if(t1 > *tmin) *tmin = t1;
        if(t2 < *tmax) *tmax = t2;

        if(*tmin > *tmax)
            return 0;
    }

    if(*tmin > ray->distance)
        return 0;

    *q = v2_add(ray->position, v2_muls(ray->direction, *tmin));

    return 1;

}

float IntersectRayTriangle2D(InterTriangleParam *tri, InterRay2DParam *ray){

    float res = 0;

    /*vec2 e1 = v2_sub(tri->p2, tri->p1);
    vec2 e2 = v2_sub(tri->p3, tri->p1);
    // Вычисление вектора нормали к плоскости
    vec2 pvec = v2_cross(ray->direction, e2);
    float det = v2_dot(e1, pvec);

    // Луч параллелен плоскости
    if (det < 1e-8 && det > -1e-8) {
        continue;
    }

    float inv_det = 1 / det;
    vec2 tvec = v2_sub(ray->position, tri->p1);
    float u = v2_dot(tvec, pvec) * inv_det;
    if (u < 0 || u > 1) {
        continue;
    }

    vec2 qvec = v2_cross(tvec, e1);
    float v = v2_dot(ray->direction, qvec) * inv_det;
    if (v < 0 || u + v > 1) {
        continue;
    }

    res = v2_dot(e2, qvec) * inv_det;*/

    return res;
}

int IntersectRayShape(void *obj, InterRay2DParam *ray)
{
    GameObject2D *shape = obj;

    vertexParam *vParam = &shape->graphObj.shape.vParam;

    InterTriangleParam triangle;

    for(int i=0; i < vParam->verticesSize;i+=3)
    {

        Vertex2D * vert = &vParam->vertices + ((i + 0) * vParam->typeSize);

        if(v2_distance(vert->position, ray->position) > ray->distance + 1.0f)
            continue;

        triangle.p1 = vert->position;
        vert = &vParam->vertices + ((i + 1) * vParam->typeSize);
        triangle.p2 = vert->position;
        vert = &vParam->vertices + ((i + 2) * vParam->typeSize);
        triangle.p3 = vert->position;

        if(IntersectRayTriangle2D(&triangle, ray))
            return true;

    }

    return 0;
}
