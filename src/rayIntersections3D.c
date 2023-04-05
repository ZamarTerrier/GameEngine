#include "rayIntersections3D.h"

#include "gameObject3D.h"

int Intersect3DRaySphere(InterRay3DParam *ray, vec3 ObjP, float *t, vec3 *q)
{
    float r = 1;
    vec3 m = v3_sub(ray->position, ObjP);
    float b = v3_dot(m, ray->direction);
    float c = v3_dot(m, m) - r * r;

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

    *q = v3_add(ray->position, v3_muls(ray->direction,*t ));

    return 1;
}

// Intersect segment S(t)=sa+t(sb-sa), 0<=t<=1 against cylinder specified by p, q and r
int Intersect3DSegmentCylinder(InterRay3DParam *ray, vec3 p, vec3 q, float r, float *t, vec3 *q_res)
{
    vec3 endPoint = v3_add(ray->position, v3_muls(ray->direction, ray->distance));

    vec3 d = v3_sub(q, p), m = v3_sub(ray->position, p), n = v3_sub(endPoint, ray->position);
    float md = v3_dot(m, d);
    float nd = v3_dot(n, d);
    float dd = v3_dot(d, d);

    // Test if segment fully outside either endcap of cylinder
    if (md < 0.0f && md + nd < 0.0f) return 0; // Segment outside ’p’ side of cylinder
    if (md > dd && md + nd > dd) return 0; // Segment outside ’q’ side of cylinder
    float nn = v3_dot(n, n);
    float mn = v3_dot(m, n);
    float a = dd * nn - nd * nd;
    float k = v3_dot(m, m) - r * r;
    float c = dd * k - md * md;

    if (fabs(a) < 0.001f) {
        // Segment runs parallel to cylinder axis
        if (c > 0.0f) return 0;
         // ’a’ and thus the segment lie outside cylinder
        // Now known that segment intersects cylinder; figure out how it intersects
        if (md < 0.0f) *t = -mn / nn;
         // Intersect segment against ’p’ endcap
        else if (md > dd) *t = (nd - mn) / nn; // Intersect segment against ’q’ endcap
        else *t = 0.0f;

        *q_res = v3_add(ray->position, v3_muls(v3_sub(endPoint, ray->position), *t));

         // ’a’ lies inside cylinder
        return 1;
    }
    float b = dd * mn - nd * md;
    float discr = b * b - a * c;
    if (discr < 0.0f) return 0;
     // No real roots; no intersection
    *t = (-b - sqrt(discr)) / a;
    if (*t < 0.0f || *t > 1.0f) return 0;
     // Intersection lies outside segment
    if (md + *t * nd < 0.0f) {
    // Intersection outside cylinder on ’p’ side
        if (nd <= 0.0f) return 0;
         // Segment pointing away from endcap
            *t = -md / nd;
        // Keep intersection if Dot(S(t) - p, S(t) - p) <= r∧ 2

        *q_res = v3_add(ray->position, v3_muls(v3_sub(endPoint, ray->position), *t));
        return k + 2 * *t * (mn + *t * nn) <= 0.0f;
    } else if (md + *t * nd > dd) {
        // Intersection outside cylinder on ’q’ side
        if (nd >= 0.0f) return 0; // Segment pointing away from endcap
        *t = (dd - md) / nd;
        // Keep intersection if Dot(S(t) - q, S(t) - q) <= r∧ 2

        *q_res = v3_add(ray->position, v3_muls(v3_sub(endPoint, ray->position), *t));
        return k + dd - 2 * md + *t * (2 * (mn - nd) + *t * nn) <= 0.0f;
    }

    *q_res = v3_add(ray->position, v3_muls(v3_sub(endPoint, ray->position), *t));
    // Segment intersects cylinder between the endcaps; t is correct
    return 1;
}

int Intersect3DRayAABB(InterRay3DParam *ray,  vec3 obj_min, vec3 obj_max, float *tmin, float *tmax, vec3 *q){

    *tmin = 0.0f;
    *tmax = 20000;

    float *d = (float *)&ray->direction;
    float *p = (float *)&ray->position;
    float *min = (float *)&obj_min;
    float *max = (float *)&obj_max;

    for(int i=0;i < 3;i++){
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

    *q = v3_add(ray->position, v3_muls(ray->direction, *tmin));

    return 1;

}

float Intersect3DRayTriangle(void* shape, InterRay3DParam *ray, vec3 *q){

    GameObject3D *model = (GameObject3D *) shape;

    vertexParam *vParam = &model->graphObj.shapes[0].vParam;
    indexParam *iParam = &model->graphObj.shapes[0].iParam;

    if(vParam->verticesSize < 3)
        return;

    vec3 origPos = model->transform.position;
    vec3 scale = model->transform.scale;

    vec3 p0, p1, p2;

    float res = 0;
    int curr = 0, num = 0, i0 = 0, i1 = 0, i2 = 0;

    Vertex3D *verts = vParam->vertices;

    for(int i=0;i < iParam->indexesSize / 3 ;i++){

        curr = i * 3;

        i0 = iParam->indices[curr];
        i1 = iParam->indices[curr + 1];
        i2 = iParam->indices[curr + 2];

        p0 = v3_add(v3_mul(verts[i0].position, scale), origPos);
        p1 = v3_add(v3_mul(verts[i1].position, scale), origPos);
        p2 = v3_add(v3_mul(verts[i2].position, scale), origPos);

        if(v3_distance(p0, ray->position) > ray->distance + 1.0f)
            continue;

        vec3 e1 = v3_sub(p1, p0);
        vec3 e2 = v3_sub(p2, p0);
        // Вычисление вектора нормали к плоскости
        vec3 pvec = v3_cross(ray->direction, e2);
        float det = v3_dot(e1, pvec);

        // Луч параллелен плоскости
        if (det < 1e-8 && det > -1e-8) {
            continue;
        }

        float inv_det = 1 / det;
        vec3 tvec = v3_sub(ray->position, p0);
        float u = v3_dot(tvec, pvec) * inv_det;
        if (u < 0 || u > 1) {
            continue;
        }

        vec3 qvec = v3_cross(tvec, e1);
        float v = v3_dot(ray->direction, qvec) * inv_det;
        if (v < 0 || u + v > 1) {
            continue;
        }

        res = v3_dot(e2, qvec) * inv_det;
        break;

    }

    if(res > ray->distance)
        return 0;

    *q = v3_add(ray->position, v3_muls(ray->direction, res));

    return res;
}
