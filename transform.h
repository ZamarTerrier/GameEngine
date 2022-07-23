#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "stdinclude.h"

typedef struct{
    vec2 position;
    vec2 rotation;
    vec2 scale;
    struct{
        vec2 offset;
        vec2 scale;
    }img;
} Transform2D;

typedef struct{
    vec3 position;
    vec3 rotation;
    vec3 scale;
    struct{
        vec2 offset;
        vec2 scale;
    }img;
} Transform3D;

struct GameObject2D;
struct GameObject3D;

void initTransform2D(Transform2D* transform);
void initTransform3D(Transform3D* transform);

void setPosTransform2D(struct GameObject2D* go, vec2 pos);
void setPosTransform3D(struct GameObject3D* go, vec3 pos);
vec2 getPosTransform2D(struct GameObject2D* go);
vec3 getPosTransform3D(struct GameObject3D* go);

void setOffsetTransform(struct GameObject2D* go, vec2 offset);
vec2 getOffsetTransform(struct GameObject2D* go);

void setScaleTransform2D(struct GameObject2D* go, vec2 scale);
void setScaleTransform3D(struct GameObject3D* go, vec3 scale);
vec2 getScaleTransform2D(struct GameObject2D* go);
vec3 getScaleTransform3D(struct GameObject3D* go);
void setRotateTransform3D(struct GameObject3D* go, vec3 rotate);
vec3 getRotateTransform3D(struct GameObject3D* go);

#endif // TRANSFORM_H
