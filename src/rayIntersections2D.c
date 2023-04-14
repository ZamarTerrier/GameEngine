#include "rayIntersections2D.h"

#include "intersections2D.h"

#include "gameObject2D.h"

#include "e_resource_data.h"

int IntersectRayCircle(InterRay2DParam *ray, InterCircleParam *circle, float *t, vec2 *q)
{
    vec2 ac = v2_sub(circle->center, ray->position);
    vec2 p2 = v2_add(ray->position, v2_muls(ray->direction, ray->distance));
    vec2 ab = v2_sub(p2, ray->position);
    float ab2 = v2_dot(ab, ab);
    float acab = v2_dot(ac, ab);
    float d = acab / ab2;
    d = (d < 0) ? 0 : d;
    d = (d > 1) ? 1 : d;
    vec2 h;
    h.x = (ab.x * d + ray->position.x) - circle->center.x;
    h.y = (ab.y * d + ray->position.y) - circle->center.y;
    float h2 = v2_dot(h, h);
    return h2 <= circle->radius * circle->radius;
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


int IntersectRayTriangle2D(InterTriangleParam *tri, InterRay2DParam *ray){

    vec2 p1 = ray->position;
    vec2 p2 = v2_add(ray->position, v2_muls(ray->direction, ray->distance));

    if(IntersectLineToLine(p1, p2, tri->p1, tri->p2))
        return true;
    else if(IntersectLineToLine(p1, p2, tri->p2, tri->p3))
        return true;
    else if(IntersectLineToLine(p1, p2, tri->p3, tri->p1))
        return true;

    return false;
}

int IntersectRayShape( InterRay2DParam *ray, void *obj)
{
    GameObject2D *shape = obj;

    vertexParam *vParam = &shape->graphObj.shapes[0].vParam;
    indexParam *iParam = &shape->graphObj.shapes[0].iParam;

    InterTriangleParam triangle;

    vec2 wind_size = {WIDTH, HEIGHT};

    for(int i=0; i < iParam->indexesSize;i+=3)
    {

        Vertex2D * vert = vParam->vertices;
        triangle.p1 = v2_mul(v2_add(vert[iParam->indices[i + 0]].position, shape->transform.position), wind_size);
        triangle.p2 = v2_mul(v2_add(vert[iParam->indices[i + 1]].position, shape->transform.position), wind_size);
        triangle.p3 = v2_mul(v2_add(vert[iParam->indices[i + 2]].position, shape->transform.position), wind_size);

        if(IntersectRayTriangle2D(&triangle, ray))
            return true;

    }

    return false;
}
