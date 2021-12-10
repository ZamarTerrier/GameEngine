#ifndef CAMERA_H
#define CAMERA_H

#include "stdinclude.h"

#include "resource.h"

typedef struct{
    vec2 pos;
    vec2 scale;
} Camera;

void initCamera();

void setViewPos(vec2 pos);

vec2 getViewPos();

void setViewScale(vec2 scale);

#endif // CAMERA_H
