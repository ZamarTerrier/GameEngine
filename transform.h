#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "stdinclude.h"

typedef struct{
    vec2 pos;
    vec2 scale;
    struct{
        vec2 offset;
        vec2 scale;
    }img;
} Transform;

void initTransform(Transform* transform);

void setPosTransform(Transform* transform, vec2 pos);

vec2 getPosTransform(Transform* transform);

void setScaleTransform(Transform* transform, vec2 scale);

vec2 getScaleTransform(Transform* transform);

#endif // TRANSFORM_H
