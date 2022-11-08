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
        vec2 origin;
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

void Transform2DSetPosition(struct GameObject2D* go, float x, float y);
vec2 Transform2DGetPosition(struct GameObject2D* go);
void Transform2DSetImageOrigin(struct GameObject2D* go, float x, float y);
vec2 Transform2DGetImageOrigin(struct GameObject2D* go);
void Transform2DReposition(struct GameObject2D* go);
void Transform2DSetImageOffset(struct GameObject2D* go, float x, float y);
vec2 Transform2DGetImageOffset(struct GameObject2D* go);
void Transform2DSetScale(struct GameObject2D* go, float x, float y);
vec2 Transform2DGetScale(struct GameObject2D* go);
void Transform2DSetImageScale(struct GameObject2D* go, float x, float y);
vec2 Transform2DGetImageScale(struct GameObject2D* go);
void Transform2DRescale(struct GameObject2D* go);

void Transform3DSetPositionT(Transform3D* transform, vec3 pos);
void Transform3DSetPosition(struct GameObject3D* go, vec3 pos);
vec3 Transform3DGetPosition(struct GameObject3D* go);

void Transform3DSetScaleT(Transform3D* transform, vec3 scale);
void Transform3DSetScale(struct GameObject3D* go, vec3 scale);
vec3 Transform3DGetScale(struct GameObject3D* go);

void Transform3DSetRotateT(Transform3D* transform, vec3 rotate);
void Transform3DSetRotate(struct GameObject3D* go, vec3 rotate);
vec3 Transform3DGetRotate(struct GameObject3D* go);

#endif // TRANSFORM_H
