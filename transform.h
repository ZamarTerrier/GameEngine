#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "stdinclude.h"

typedef struct{
    vec2 position;
    vec2 positionOrig;
    vec2 rotation;
    vec2 scale;
    vec2 scaleOrig;
    struct{
        vec2 offset;
        vec2 scale;
    }img;
    mat4 model;
} Transform2D;

typedef struct{
    vec3 position;
    vec3 rotation;
    vec3 scale;
    struct{
        vec2 offset;
        vec2 scale;
    }img;
    mat4 model;
} Transform3D;

struct GameObject2D;
struct GameObject3D;

void Transform2DInit(Transform2D* transform);
void Transform3DInit(Transform3D* transform);

void Transform2DSetPosition(struct GameObject2D* go, vec2 pos);
void Transform2DReposition(struct GameObject2D* go);
void Transform3DSetPosition(struct GameObject3D* go, vec3 pos);
vec2 Transform2DGetPosition(struct GameObject2D* go);
vec3 Transform3DGetPosition(struct GameObject3D* go);

void Transform2DSetOffset(struct GameObject2D* go, vec2 offset);
vec2 Transform2DGetOffset(struct GameObject2D* go);

void Transform2DSetScale(struct GameObject2D* go, vec2 scale);
void Transform2DRescale(struct GameObject2D* go);
void Transform3DSetScale(struct GameObject3D* go, vec3 scale);
vec2 Transform2DGetScale(struct GameObject2D* go);
vec3 Transform3DGetScale(struct GameObject3D* go);
void Transform3DSetRotate(struct GameObject3D* go, vec3 rotate);
vec3 Transform3DGetRotate(struct GameObject3D* go);

#endif // TRANSFORM_H
