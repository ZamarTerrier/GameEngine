#ifndef CAMERA_H
#define CAMERA_H

#include "stdinclude.h"

#include "resource.h"

typedef struct{
    vec3 position;
    vec3 rotation;
    vec3 scale;
} Camera;

void initCamera();

void setViewPos2D(vec2 pos);
void setViewPos(vec3 pos);

vec2 getViewPos2D();
vec3 getViewPos();

void setViewScale2D(vec2 scale);
void setViewScale(vec3 scale);

#endif // CAMERA_H
