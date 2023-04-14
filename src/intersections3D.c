#include "intersections3D.h"

#include "gameObject3D.h"
#include "terrain_object.h"

#include "e_math.h"

float Clamp(float n, float min, float max) {
    if (n < min) return min;
    if (n > max) return max;
    return n;
}

float SqDistPointSegment(vec3 a, vec3 b, vec3 c)
{
    vec3 ab = v3_sub(b, a);
    vec3 ac = v3_sub(c, a);
    vec3 bc = v3_sub(c, b);
    float e = v3_dot(ac, ab);
    // Handle cases where c projects outside ab
    if (e <= 0.0f) return v3_dot(ac, ac);
    float f = v3_dot(ab, ab);
    if (e >= f) return v3_dot(bc, bc);
    // Handle cases where c projects onto ab
    return v3_dot(ac, ac) - e * e / f;
}

// Computes closest points C1 and C2 of S1(s)=P1+s*(Q1-P1) and
// S2(t)=P2+t*(Q2-P2), returning s and t. Function result is squared
// distance between between S1(s) and S2(t)
float ClosestPtSegmentSegment(vec3 p1, vec3 q1, vec3 p2, vec3 q2, float *s, float *t, vec3 *c1, vec3 *c2)
{
    vec3 d1 = v3_sub(q1, p1); // Direction vector of segment S1
    vec3 d2 = v3_sub(q2, p2); // Direction vector of segment S2
    vec3 r = v3_sub(p1, p2);
    float a = v3_dot(d1, d1); // Squared length of segment S1, always nonnegative
    float e = v3_dot(d2, d2); // Squared length of segment S2, always nonnegative
    float f = v3_dot(d2, r);
    // Check if either or both segments degenerate into points
    if (a <= 0.001f && e <= 0.001f ) {
        // Both segments degenerate into points
        *s = *t = 0.0f;
        *c1 = p1;
        *c2 = p2;
        return v3_dot(v3_sub(*c1, *c2), v3_sub(*c1, *c2));
    }
    if (a <= 0.001f ) {
        // First segment degenerates into a point
        *s = 0.0f;
        *t = f / e; // *s = 0 => t = (b*s + f) / e = f / e
        *t = Clamp(*t, 0.0f, 1.0f);
    } else {
        float c = v3_dot(d1, r);
        if (e <= 0.001f) {
            // Second segment degenerates into a point
            *t = 0.0f;
            *s = Clamp(-c / a, 0.0f, 1.0f); // t = 0 => *s = (b*t - c) / a = -c / a
        } else {
            // The general nondegenerate case starts here
            float b = v3_dot(d1, d2);
            float denom = a*e-b*b; // Always nonnegative
            // If segments not parallel, compute closest point on L1 to L2 and
            // clamp to segment S1. Else pick arbitrary *s (here 0)
            if (denom != 0.0f) {
                *s = Clamp((b*f - c*e) / denom, 0.0f, 1.0f);
            } else
                *s = 0.0f;

            // Compute point on L2 closest to S1(s) using
            // t = Dot((P1 + D1*s) - P2,D2) / Dot(D2,D2) = (b*s + f) / e
            *t = (b * (*s) + f) / e;
            //If t in [0,1] done. Else clamp t, recompute *s for the new value
            //of t using *s = Dot((P2 + D2*t) - P1,D1) / Dot(D1,D1)= (t*b - c) / a
            //and clamp *s to [0, 1]
            if(*t < 0.0f) {
            *t = 0.0f;
            *s = Clamp(-c / a, 0.0f, 1.0f);
            } else if (*t > 1.0f) {
            *t = 1.0f;
            *s = Clamp((b - c) / a, 0.0f, 1.0f);
            }
        }
    }

    *c1 = v3_add(p1, v3_muls(d1, *s));
    *c2 =v3_add( p2, v3_muls(d2, *t));
    return v3_dot(v3_sub(*c1, *c2), v3_sub(*c1, *c2));
}

vec3 ClosestPtPointTriangle3D(vec3 p, vec3 p0, vec3 p1, vec3 p2){

    vec3 ab = v3_sub(p1, p0);
    vec3 ac = v3_sub(p2, p0);
    vec3 ap = v3_sub(p, p0);

    float d1 = v3_dot(ab, ap);
    float d2 = v3_dot(ac, ap);
    if (d1 <= 0.0f && d2 <= 0.0f)
        return p0;

    vec3 bp = v3_sub(p, p1);
    float d3 = v3_dot(ab, bp);
    float d4 = v3_dot(ac, bp);
    if (d3 >= 0.0f && d4 <= d3)
        return p1;

    float vc = d1*d4 - d3*d2;
    if(vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f){
        float v = d1 / (d1 - d3);
        return v3_add(p0, v3_muls(ab, v));
    }

    vec3 cp = v3_sub(p, p2);
    float d5 = v3_dot(ab, cp);
    float d6 = v3_dot(ac, cp);
    if (d6 >= 0.0f && d5 <= d6) return p2;

    float vb = d5*d2 - d1*d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
        float w = d2 / (d2 - d6);
        return v3_add(p0,v3_muls(ac, w)); // barycentric coordinates (1-w,0,w)
    }
    // Check if P in edge region of BC, if so return projection of P onto BC
    float va = d3*d6 - d5*d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
        float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return v3_add(p1,v3_muls(v3_sub(p2, p1), w)); // barycentric coordinates (0,1-w,w)
    }
    // P inside face region. Compute Q through its barycentric coordinates (u,v,w)
    float denom = 1.0f / (va + vb + vc);
    float v = vb * denom;
    float w = vc * denom;

    return v3_add(p0, v3_add(v3_muls(ab, v), v3_muls(ac, w)));
}

int IntersectionSpherePlane(void *obj1, void *obj2, float *distance, float *depth, vec3 *dir)
{
    InterPlaneParam *plane = obj1;
    InterSphereParam *sphere = obj2;

    float dist = v3_dot(sphere->center, plane->normal) - v3_dot(plane->normal, plane->position);

    if(dist <= sphere->radius * sphere->radius)
    {
        *distance = dist;
        *depth = (sphere->radius  * sphere->radius) - dist;
        *dir = v3_muls(plane->normal, -*depth);
        return true;
    }

    dir->x = 0;
    dir->y = 0;
    dir->z = 0;

    *depth = 0;

    return false;
}

int IntersectionSphereTerrain(void *sph_obj, void *terr_obj, float *distance, float *depth, vec3 *dir)
{
    InterSphereParam *sphere = sph_obj;
    TerrainObject *terrain = terr_obj;

    dir->x = 0;
    dir->y = 0;
    dir->z = 0;

    *depth = 0;

    vec3 pos = sphere->center;

    if(pos.x < 0 || pos.z < 0)
        return false;

    int pos_x = floor(pos.x * terrain->width) / terrain->width;
    int pos_z = floor(pos.z * terrain->height) / terrain->height;

    TerrainVertex *verts = terrain->go.graphObj.shapes[0].vParam.vertices;

    vec3 some_pos = verts[pos_x * (terrain->width + 1) + pos_z].position;

    float dist = v3_distance(pos, some_pos);

    if(dist <= sphere->radius * sphere->radius)
    {

        sphere->center.y = some_pos.y + (sphere->radius * sphere->radius);
        return true;
    }


    return false;

}

float SqDistPointAABB(vec3 pos, void *obj)
{
    InterAABBParam *box = (InterAABBParam *) obj;
    vec3 up = {box->position.x + box->size, box->position.y + box->size, box->position.z + box->size};
    vec3 down = {box->position.x - box->size, box->position.y - box->size, box->position.z - box->size};

    float *p = &pos;
    float *max = &up;
    float *min = &down;

    float sqDist = 0.0f;
    for (int i = 0; i < 3; i++) {
        // For each axis count any excess distance outside box extents
        float v = p[i];
        if (v < min[i]) sqDist += (min[i] - v) * (min[i] - v);
        if (v > max[i]) sqDist += (v - max[i]) * (v - max[i]);
    }
    return sqDist;
}

void ClosestPtPointAABB(vec3 pos, void *obj, vec3 *res)
{
    InterAABBParam *box = (InterAABBParam *) obj;
    vec3 up = {box->position.x + box->size, box->position.y + box->size, box->position.z + box->size};
    vec3 down = {box->position.x - box->size, box->position.y - box->size, box->position.z - box->size};

    float *p = &pos;
    float *max = &up;
    float *min = &down;
    float *q = res;

    for(int i=0; i < 3; i++)
    {
        float v = p[i];
        if(v < min[i]) v = min[i];
        if(v > max[i]) v = max[i];
        q[i] = v;
    }
}

int IntersectionSphereTriangle(vec3 sPos, float r, vec3 p0, vec3 p1, vec3 p2, vec3 *resPos, float *dist, float *depth, vec3 *dir)
{
    // Find point P on triangle ABC closest to sphere center
    *resPos = ClosestPtPointTriangle3D(sPos, p0, p1, p2);
    vec3 v = v3_sub(*resPos, sPos);

    *dist = v3_dot(v,v);

    int intr = *dist <= r * r;

    *dir = intr ? v3_add(*dir, v3_muls(v3_norm(v), -0.1f)) : *dir;
    *depth = intr ? r * r - *dist : 0;
    return intr;
}

int IntersectionCapsuleShape(void *obj1, void *shape, float *dist, float *depth, vec3 *dir){

    GameObject3D *model = (GameObject3D *)shape;
    InterCapsuleParam *obj = (InterCapsuleParam *)obj1;

    vertexParam *vParam = &model->graphObj.shapes[0].vParam;
    indexParam *iParam = &model->graphObj.shapes[0].iParam;

    if(vParam->verticesSize <= 0)
        return;

    vec3 origPos = model->transform.position;
    vec3 scale = model->transform.scale;

    vec3 temp, summ = {0, 0, 0};
    vec3 capsule_a = {obj->position.x, obj->position.y - obj->height, obj->position.z};
    vec3 capsule_b = {obj->position.x, obj->position.y + obj->height, obj->position.z};

    vec3 p0, p1, p2, dir_a, dir_b;

    float r = obj->radius * 0.58f, t_a, t_b, depth_a, depth_b;

    int curr = 0, res = 0, buff_a = 0, buff_b = 0, cou = 0;

    Vertex3D *verts = vParam->vertices;
    for(int i=0;i < vParam->verticesSize;i++){

        curr = i * 3;

        p0 = v3_add(v3_mul(verts[iParam->indices[curr]].position, scale), origPos);
        p1 = v3_add(v3_mul(verts[iParam->indices[curr + 1]].position, scale), origPos);
        p2 = v3_add(v3_mul(verts[iParam->indices[curr + 2]].position, scale), origPos);

        if(v3_distance(p0, origPos) > r * r + 1.0f)
            continue;

        buff_a =  IntersectionSphereTriangle(capsule_a, r, p0, p1, p2, &temp, &t_a, &depth_a, &dir_a);

        buff_b = IntersectionSphereTriangle(capsule_b, r, p0, p1, p2, &temp, &t_b, &depth_b, &dir_b);

        if(buff_a > 0 || buff_b > 0){
            cou ++;
            *dist = t_a > t_b ? t_b : t_a;
            *depth = t_a > t_b ? depth_b : depth_a;
            *dir = t_a > t_b ? dir_b : dir_a;
            res = t_a > t_b ? buff_b : buff_a;
            summ = v3_add(summ, *dir);
        }
    }


    *dir = v3_divs(summ, 20);

    return res;
}

int IntersectionSphereShape(void *obj1, void *shape, float *dist, float *depth, vec3 *dir){

    GameObject3D *model = (GameObject3D *)shape;
    InterSphereParam *obj = (InterSphereParam *)obj1;

    vertexParam *vParam = &model->graphObj.shapes[0].vParam;
    indexParam *iParam = &model->graphObj.shapes[0].iParam;

    if(vParam->verticesSize <= 0)
        return;

    vec3 origPos = model->transform.position;
    vec3 scale = model->transform.scale;

    vec3 summ = {0, 0, 0}, tempPos;

    float r = obj->radius * 0.58f;

    vec3 p0, p1, p2;

    int curr = 0, res = 0, buff = 0, cou = 0;

    Vertex3D *verts = vParam->vertices;
    for(int i=0;i < vParam->verticesSize;i++){

        curr = i * 3;

        p0 = v3_add(v3_mul(verts[iParam->indices[curr]].position, scale), origPos);
        p1 = v3_add(v3_mul(verts[iParam->indices[curr + 1]].position, scale), origPos);
        p2 = v3_add(v3_mul(verts[iParam->indices[curr + 2]].position, scale), origPos);

        buff = IntersectionSphereTriangle(obj->center, r, p0, p1, p2, &tempPos, dist, depth, dir);

        if(buff > 0){
            cou ++;
            res = buff;
            summ = v3_add(summ, *dir);
        }

    }

    *dir = v3_divs(summ, 20);

    return res;
}

int IntersectionSphereToSphere(void *obj1, void *obj2, float *dist, float *depth, vec3 *dir){


    InterSphereParam *o1 = (InterSphereParam *)obj1;
    InterSphereParam *o2 = (InterSphereParam *)obj2;

    float r1 = o1->radius * 0.58f, r2 = o2->radius * 0.58f;

    vec3 v = v3_sub(o2->center, o1->center);

    *dist = v3_dot(v, v);

    float rSumm = r1 + r2;

    int intr = *dist <= rSumm * rSumm;

    *dir = intr ? v3_muls(v3_norm(v), -1) : *dir;

    *depth = intr ? rSumm * rSumm - *dist : 0;

    return intr;

}

int IntersectionSphereAABB(void *obj1, void *obj2, float *dist, float *depth, vec3 *dir)
{

    InterSphereParam *sph = (InterSphereParam *)obj1;
    InterAABBParam *box = (InterAABBParam *)obj2;

    vec3 point;

    ClosestPtPointAABB(sph->center, box, &point);

    float sqDist = v3_distance(point, sph->center);

    vec3 v = v3_sub(point, sph->center);

    *dist = sqDist;
    *dir = v3_muls(v3_norm(v), -1);
    float r = sph->radius * 0.58f;
    *depth = r * r - *dist;
    // Sphere and AABB intersect if the (squared) distance
    // between them is less than the (squared) sphere radius
    return sqDist <= r * r;

}

int IntersectionSphereCapsule(void *obj1, void *obj2, float *dist, float *depth, vec3 *dir)
{
    InterSphereParam *sph = (InterSphereParam *) obj1;
    InterCapsuleParam *cap = (InterCapsuleParam *) obj2;

    vec3 capsule_a = {cap->position.x, cap->position.y + cap->height, cap->position.z};
    vec3 capsule_b = {cap->position.x, cap->position.y - cap->height, cap->position.z};


    vec3 v = v3_sub(cap->position, sph->center);

    // If (squared) distance smaller than (squared) sum of radii, they collide
    float radius = sph->radius * 0.58f + cap->radius * 0.58f;

    // Compute (squared) distance between sphere center and capsule line segment
    float dist2 = SqDistPointSegment(capsule_a, capsule_b, sph->center);

    *dist = dist2;
    *dir = v3_muls(v3_norm(v), -1);
    *depth = radius * radius - *dist;


    return dist2 <= radius * radius;
}

int IntersectionCapsuleCapsule(void *obj1, void *obj2, float *dist, float *depth, vec3 *dir)
{
    InterCapsuleParam *cap1 = (InterCapsuleParam *) obj1;
    InterCapsuleParam *cap2 = (InterCapsuleParam *) obj2;

    vec3 capsule1_a = {cap1->position.x, cap1->position.y - cap1->height, cap1->position.z};
    vec3 capsule1_b = {cap1->position.x, cap1->position.y + cap1->height, cap1->position.z};

    vec3 capsule2_a = {cap2->position.x, cap2->position.y - cap2->height, cap2->position.z};
    vec3 capsule2_b = {cap2->position.x, cap2->position.y + cap2->height, cap2->position.z};


    // Compute (squared) distance between the inner structures of the capsules
    float s, t;
    vec3 c1, c2;
    float dist2 = ClosestPtSegmentSegment(capsule1_a, capsule1_b, capsule2_a, capsule2_b, &s, &t, &c1, &c2);
    // If (squared) distance smaller than (squared) sum of radii, they collide
    float radius = cap1->radius + cap2->radius;

    vec3 v = v3_sub(cap1->position, cap2->position);

    *dist = dist2;
    *dir = v3_muls(v3_norm(v), -1);
    *depth = radius * radius - *dist;

    return dist2 <= radius * radius;
}

int IntersectionAABBPlane(void *obj1, void *obj2, float *dist, float *depth, vec3 *dir)
{
    InterAABBParam* box = (InterAABBParam *)obj1;
    InterPlaneParam* plane = (InterAABBParam *)obj2;

    vec3 max = {box->position.x + box->size, box->position.y + box->size, box->position.z + box->size};
    vec3 min = {box->position.x - box->size, box->position.y - box->size, box->position.z - box->size};

    // These two lines not necessary with a (center, extents) AABB representation
    vec3 c = v3_muls(v3_add(max, min), 0.5f); // Compute AABB center
    vec3 e = v3_sub(max, c); // Compute positive extents
    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    float r = e.x*fabs(plane->normal.x) + e.y*fabs(plane->normal.y) + e.z*fabs(plane->normal.z);
    // Compute distance of box center from plane
    float s = v3_dot(plane->normal, c) - v3_dot(plane->normal, plane->position);

    *dist = s;
    *depth = r - s;
    *dir = plane->normal;
    // Intersection occurs when distance s falls within [-r,+r] interval
    return fabs(s) <= r;
}

int IntersectionAABBAABB(void *obj1, void *obj2, float *dist, float *depth, vec3 *dir)
{
    InterAABBParam* box1 = (InterAABBParam *)obj1;
    InterAABBParam* box2 = (InterAABBParam *)obj2;

    vec3 min1 = {box1->position.x - box1->size, box1->position.y - box1->size, box1->position.z - box1->size};
    vec3 max1 = {box1->position.x + box1->size, box1->position.y + box1->size, box1->position.z + box1->size};

    vec3 min2 = {box2->position.x - box2->size, box2->position.y - box2->size, box2->position.z - box2->size};
    vec3 max2 = {box2->position.x + box2->size, box2->position.y + box2->size, box2->position.z + box2->size};

    vec3 v = v3_sub(box2->position, box1->position);

    if (max1.x < min2.x || min1.x > max2.x) return 0;
    if (max1.y < min2.y || min1.y > max2.y) return 0;
    if (max1.z < min2.z || min1.z > max2.z) return 0;

    *dist = SqDistPointAABB(box1->position, box2);
    *dir = v3_muls(v3_norm(v), -1);
    *depth = 0.01f;


    return 1;
}

int IntersectionAABBCapsule(void *obj1, void *obj2, float *dist, float *depth, vec3 *dir){

    InterCapsuleParam* cap = (InterCapsuleParam *)obj1;
    InterAABBParam* box = (InterAABBParam *)obj2;

    vec3 a = {cap->position.x, cap->position.y - cap->height, cap->position.z};
    vec3 b = {cap->position.x, cap->position.y + cap->height, cap->position.z};

    float d1 = SqDistPointAABB(a, box);
    float d2 = SqDistPointAABB(b, box);

    vec3 v = v3_sub(box->position, cap->position);

    float r = cap->radius * 0.58f;

    float d = e_max(d1, d2);

    d = d > 1.0 ? 0 : d;

    *dist = d;
    *dir = v3_muls(v3_norm(v), -1);
    *depth = r * r - *dist;

    return d1 <= r * r || d2 <= r * r;
}

int IntersectionTriangleAABB(vec3 v0, vec3 v1, vec3 v2, void *obj, float *dist, float *depth, vec3 *dir)
{
    float p0, p1, p2, r;
    InterAABBParam *box = (InterAABBParam *)obj;
    vec3 min_t = {box->position.x - box->size, box->position.y - box->size, box->position.z - box->size};
    vec3 max_t = {box->position.x + box->size, box->position.y + box->size, box->position.z + box->size};
    // Compute box center and extents (if not already given in that format)
    vec3 c = v3_muls(v3_add(min_t, max_t), 0.5f);
    float e0 = (max_t.x - min_t.x) * 0.5f;
    float e1 = (max_t.y - min_t.y) * 0.5f;
    float e2 = (max_t.z - min_t.z) * 0.5f;
    // Translate triangle as conceptually moving AABB to origin
    v0 = v3_sub(v0, c);
    v1 = v3_sub(v1, c);
    v2 = v3_sub(v2, c);
    // Compute edge vectors for triangle
    vec3 f0 = v3_sub(v1, v0), f1 = v3_sub(v2, v1), f2 = v3_sub(v0, v2);
    // Test axes a00..a22 (category 3)
    // Test axis a00
    p0 = v0.z*v1.y - v0.y*v1.z;
    p2 = v2.z*(v1.y - v0.y) - v2.z*(v1.z - v0.z);
    r = e1 * fabs(f0.z) + e2 * fabs(f0.y);
    if (e_max(-e_max(p0, p2), e_min(p0, p2)) > r) return 0; // Axis is a separating axis
    // Repeat similar tests for remaining axes a01..a22
    // Test the three axes corresponding to the face normals of AABB b (category 1).
    // Exit if...
    // ... [-e0, e0] and [min(v0.x,v1.x,v2.x), max(v0.x,v1.x,v2.x)] do not overlap
    if (e_max(e_max(v0.x, v1.x), v2.x) < -e0 || e_min(e_min(v0.x, v1.x), v2.x) > e0) return 0;
    // ... [-e1, e1] and [min(v0.y,v1.y,v2.y), max(v0.y,v1.y,v2.y)] do not overlap
    if (e_max(e_max(v0.y, v1.y), v2.y) < -e1 || e_min(e_min(v0.y, v1.y), v2.y) > e1) return 0;
    // ... [-e2, e2] and [min(v0.z,v1.z,v2.z), max(v0.z,v1.z,v2.z)] do not overlap
    if (e_max(e_max(v0.z, v1.z), v2.z) < -e2 || e_min(e_min(v0.z, v1.z), v2.z) > e2) return 0;
    // Test separating axis corresponding to triangle face normal (category 2)
    vec3 n;
    float d;
    InterPlaneParam plane;
    plane.normal = v3_cross(f0, f1);
    plane.position = v0;

    return IntersectionAABBPlane(obj, &plane, dist, depth, dir);
}

