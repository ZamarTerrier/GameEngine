#include "intersections2D.h"

#include "gameObject2D.h"

#include "math.h"

float SqDistPointSquare(vec2 pos, InterSquareParam *box)
{
    vec2 up = {box->position.x + box->size, box->position.y + box->size};
    vec2 down = {box->position.x - box->size, box->position.y - box->size};

    float *p = &pos;
    float *max = &up;
    float *min = &down;

    float sqDist = 0.0f;
    for (int i = 0; i < 2; i++) {
        // For each axis count any excess distance outside box extents
        float v = p[i];
        if (v < min[i]) sqDist += (min[i] - v) * (min[i] - v);
        if (v > max[i]) sqDist += (v - max[i]) * (v - max[i]);
    }
    return sqDist;
}

int ComputeSideSquare(vec2 p, vec2 min, vec2 max)
{
    int code = 0;  // initialised as being inside of clip window

    if (p.x < min.x)           // to the left of clip window
        code |= 1;
    else if (p.x > max.x)      // to the right of clip window
        code |= 2;
    if (p.y < min.y)           // below the clip window
        code |= 4;
    else if (p.y > max.y)      // above the clip window
        code |= 8;

    return code;
}


bool ComputeTwoTrianles(InterTriangleParam t1, InterTriangleParam t2)
{
    vec2 da = v2_sub(t1.p1, t2.p3);
    vec2 db = v2_sub(t1.p2, t2.p3);
    vec2 dc = v2_sub(t1.p3, t2.p3);
    float dX21 = t2.p3.x - t2.p2.x;
    float dY12 = t2.p2.y - t2.p3.y;
    float D = dY12 * (t2.p1.x - t2.p3.x) + dX21 * (t2.p1.y - t2.p3.y);
    float sa = dY12 * da.x + dX21 * da.y;
    float sb = dY12 * db.x + dX21 * db.y;
    float sc = dY12 * dc.x + dX21 * db.y;
    float ta = (t2.p3.y - t2.p1.y) * da.x + (t2.p1.x - t2.p3.x) * da.y;
    float tb = (t2.p3.y - t2.p1.y) * db.x + (t2.p1.x - t2.p3.x) * db.y;
    float tc = (t2.p3.y - t2.p1.y) * dc.x + (t2.p1.x - t2.p3.x) * dc.y;
    if (D < 0) return ((sa >= 0 && sb >= 0 && sc >= 0) ||
                       (ta >= 0 && tb >= 0 && tc >= 0) ||
                       (sa+ta <= D && sb+tb <= D && sc+tc <= D));
    return ((sa <= 0 && sb <= 0 && sc <= 0) ||
          (ta <= 0 && tb <= 0 && tc <= 0) ||
          (sa+ta >= D && sb+tb >= D && sc+tc >= D));
}

bool IntersectionSegmentSquare(vec2 p1, vec2 p2, InterSquareParam *box)
{
    vec2 max_i = {box->position.x + box->size, box->position.y + box->size };
    vec2 min_i = {box->position.x - box->size, box->position.y - box->size };

    int outcode0 = ComputeSideSquare(p1, min_i, max_i);
    int outcode1 = ComputeSideSquare(p2, min_i, max_i);
        bool accept = false;

        while (true) {
            if (!(outcode0 | outcode1)) {
                // bitwise OR is 0: both points inside window; trivially accept and exit loop
                accept = true;
                break;
            } else if (outcode0 & outcode1) {
                // bitwise AND is not 0: both points share an outside zone (LEFT, RIGHT, TOP,
                // or BOTTOM), so both must be outside window; exit loop (accept is false)
                break;
            } else {
                // failed both tests, so calculate the line segment to clip
                // from an outside point to an intersection with clip edge
                double x, y;

                // At least one endpoint is outside the clip rectangle; pick it.
                int outcodeOut = outcode1 > outcode0 ? outcode1 : outcode0;

                // Now find the intersection point;
                // use formulas:
                //   slope = (y1 - y0) / (x1 - x0)
                //   x = x0 + (1 / slope) * (ym - y0), where ym is ymin or ymax
                //   y = y0 + slope * (xm - x0), where xm is xmin or xmax
                // No need to worry about divide-by-zero because, in each case, the
                // outcode bit being tested guarantees the denominator is non-zero
                if (outcodeOut & 8) {           // point is above the clip window
                    x = p1.x + (p2.x - p1.x) * (max_i.y - p1.y) / (p2.y - p1.y);
                    y = max_i.y;
                } else if (outcodeOut & 4) { // point is below the clip window
                    x = p1.x + (p2.x - p1.x) * (min_i.y - p1.y) / (p2.y - p1.y);
                    y = min_i.y;
                } else if (outcodeOut & 2) {  // point is to the right of clip window
                    y = p1.y + (p2.y - p1.y) * (max_i.x - p1.x) / (p2.x - p1.x);
                    x = max_i.x;
                } else if (outcodeOut & 1) {   // point is to the left of clip window
                    y = p1.y + (p2.y - p1.y) * (min_i.x - p1.x) / (p2.x - p1.x);
                    x = min_i.x;
                }

                // Now we move outside point to intersection point to clip
                // and get ready for next pass.
                if (outcodeOut == outcode0) {
                    p1.x = x;
                    p1.y = y;
                    outcode0 = ComputeSideSquare(p1, min_i, max_i);
                } else {
                    p2.x = x;
                    p2.y = y;
                    outcode1 = ComputeSideSquare(p2, min_i, max_i);
                }
            }
        }
        return accept;
}

vec2 ClosestPtPointTriangle2D(vec2 p, vec2 p0, vec2 p1, vec2 p2){

    vec2 ab = v2_sub(p1, p0);
    vec2 ac = v2_sub(p2, p0);
    vec2 ap = v2_sub(p, p0);

    float d1 = v2_dot(ab, ap);
    float d2 = v2_dot(ac, ap);
    if (d1 <= 0.0f && d2 <= 0.0f)
        return p0;

    vec2 bp = v2_sub(p, p1);
    float d3 = v2_dot(ab, bp);
    float d4 = v2_dot(ac, bp);
    if (d3 >= 0.0f && d4 <= d3)
        return p1;

    float vc = d1*d4 - d3*d2;
    if(vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f){
        float v = d1 / (d1 - d3);
        return v2_add(p0, v2_muls(ab, v));
    }

    vec2 cp = v2_sub(p, p2);
    float d5 = v2_dot(ab, cp);
    float d6 = v2_dot(ac, cp);
    if (d6 >= 0.0f && d5 <= d6) return p2;

    float vb = d5*d2 - d1*d6;
    if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
        float w = d2 / (d2 - d6);
        return v2_add(p0,v2_muls(ac, w)); // barycentric coordinates (1-w,0,w)
    }
    // Check if P in edge region of BC, if so return projection of P onto BC
    float va = d3*d6 - d5*d4;
    if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
        float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
        return v2_add(p1,v2_muls(v2_sub(p2, p1), w)); // barycentric coordinates (0,1-w,w)
    }
    // P inside face region. Compute Q through its barycentric coordinates (u,v,w)
    float denom = 1.0f / (va + vb + vc);
    float v = vb * denom;
    float w = vc * denom;

    return v2_add(p0, v2_add(v2_muls(ab, v), v2_muls(ac, w)));

}

int IntersectionSquarePlane(InterSquareParam *box, vec2 n, float d)
{
    vec2 max = {box->position.x + box->size, box->position.y + box->size };
    vec2 min = {box->position.x - box->size, box->position.y - box->size };

    // These two lines not necessary with a (center, extents) AABB representation
    vec2 c = v2_muls(v2_add(max, min), 0.5f); // Compute AABB center
    vec2 e = v2_sub(max, c); // Compute positive extents
    // Compute the projection interval radius of b onto L(t) = b.c + t * p.n
    float r = e.x*fabs(n.x) + e.y*fabs(n.y);
    // Compute distance of box center from plane
    float s = v2_dot(n, c) - d;
    // Intersection occurs when distance s falls within [-r,+r] interval
    return fabs(s) <= r;
}

int IntersectionCircleTriangle(vec2 sPos, float r, vec2 p0, vec2 p1, vec2 p2, vec2 *resPos, float *dist, float *depth, vec2 *dir)
{
    // Find point P on triangle ABC closest to sphere center
    *resPos = ClosestPtPointTriangle2D(sPos, p0, p1, p2);
    vec2 v = v2_sub(*resPos, sPos);

    *dist = v2_dot(v,v);

    int intr = *dist <= r * r;

    *dir = intr ? v2_add(*dir, v2_muls(v2_norm(v), -0.1f)) : *dir;
    *depth = intr ? r * r - *dist : 0;
    return intr;
}

int IntersectionCircleCircle(InterCircleParam *o1, InterCircleParam *o2, float *dist, float *depth, vec2 *dir)
{
    float r1 = o1->radius * 0.58f, r2 = o2->radius * 0.58f;

    vec2 v = v2_sub(o2->center, o1->center);

    *dist = v2_dot(v, v);

    float rSumm = r1 + r2;

    int intr = *dist <= rSumm * rSumm;

    *dir = intr ? v2_muls(v2_norm(v), -1) : *dir;

    *depth = intr ? rSumm * rSumm - *dist : 0;

    return intr;
}

int IntersectionCircleSquare(InterCircleParam *circle, InterSquareParam *quad, float *dist, float *depth, vec2 *dir)
{
    float sqDist = SqDistPointSquare(circle->center, quad);

    vec2 v = v2_sub(quad->position, circle->center);

    *dist = sqDist;
    *dir = v2_muls(v2_norm(v), -1);
    float r = circle->radius;
    *depth = r * r - *dist;
    // Sphere and AABB intersect if the (squared) distance
    // between them is less than the (squared) sphere radius
    return sqDist <= r * r;

}

int IntersectionSquareSquare(InterSquareParam *box1, InterSquareParam *box2, float *dist, float *depth, vec2 *dir)
{
    vec2 min1 = {box1->position.x - box1->size, box1->position.y - box1->size};
    vec2 max1 = {box1->position.x + box1->size, box1->position.y + box1->size};

    vec2 min2 = {box2->position.x - box2->size, box2->position.y - box2->size};
    vec2 max2 = {box2->position.x + box2->size, box2->position.y + box2->size};

    vec2 v = v2_sub(box2->position, box1->position);

    if (max1.x < min2.x || min1.x > max2.x) return 0;
    if (max1.y < min2.y || min1.y > max2.y) return 0;

    *dist = SqDistPointSquare(box1->position, box2);
    *dir = v2_muls(v2_norm(v), -1);
    *depth = 0.01f;

    return 1;
}

int IntersectionTriangleSquare(InterTriangleParam triangle, InterSquareParam *box)
{
    vec2 min_t = {box->position.x - box->size, box->position.y - box->size };
    vec2 max_t = {box->position.x + box->size, box->position.y + box->size };
    // Compute box center and extents (if not already given in that format)
    vec2 c = v2_muls(v2_add(min_t, max_t), 0.5f);

    // Translate triangle as conceptually moving AABB to origin
    vec2 v0 = v2_sub(triangle.p1, c);
    vec2 v1 = v2_sub(triangle.p2, c);
    vec2 v2 = v2_sub(triangle.p3, c);


    bool r1 = IntersectionSegmentSquare(v0, v1, box);
    bool r2 = IntersectionSegmentSquare(v1, v2, box);
    bool r3 = IntersectionSegmentSquare(v2, v0, box);

    if(r1 && r2 && r3)
        return true;
    else
        return false;
}

int IntersectionSphapeSquare(void *obj1, InterSquareParam *square)
{
    GameObject2D *shape1 = obj1;

    vertexParam *vParam1 = &shape1->graphObj.shape.vParam;

    InterTriangleParam triangle;

    for(int i=0;i<vParam1->verticesSize; i+=3)
    {
        Vertex2D * vert = &vParam1->vertices + ((i + 0) * vParam1->typeSize);
        triangle.p1 = vert->position;
        vert = &vParam1->vertices + ((i + 1) * vParam1->typeSize);
        triangle.p2 = vert->position;
        vert = &vParam1->vertices + ((i + 2) * vParam1->typeSize);
        triangle.p3 = vert->position;

        if(IntersectionTriangleSquare(triangle, square))
            return true;
    }

    return false;
}

int IntersectionShapeShape(void *obj1, void *obj2)
{
    GameObject2D *shape1 = obj1;
    GameObject2D *shape2 = obj2;

    vertexParam *vParam1 = &shape1->graphObj.shape.vParam;
    vertexParam *vParam2 = &shape2->graphObj.shape.vParam;

    InterTriangleParam triangle1, triangle2;

    for(int i=0;i<vParam1->verticesSize; i+=3)
    {
        Vertex2D * vert = &vParam1->vertices + ((i + 0) * vParam1->typeSize);
        triangle1.p1 = vert->position;
        vert = &vParam1->vertices + ((i + 1) * vParam1->typeSize);
        triangle1.p2 = vert->position;
        vert = &vParam1->vertices + ((i + 2) * vParam1->typeSize);
        triangle1.p3 = vert->position;
        for(int j=0;j < vParam2->verticesSize; j+=3)
        {
            vert = &vParam2->vertices + ((i + 0) * vParam2->typeSize);
            triangle2.p1 = vert->position;
            vert = &vParam2->vertices + ((i + 1) * vParam2->typeSize);
            triangle2.p2 = vert->position;
            vert = &vParam2->vertices + ((i + 2) * vParam2->typeSize);
            triangle2.p3 = vert->position;

            if(ComputeTwoTrianles(triangle1, triangle2))
                return true;

        }
    }

    return false;
}

