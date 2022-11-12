#ifndef CAMERA_H
#define CAMERA_H

#include "engine_includes.h"

#include "e_resource.h"

typedef struct{
    vec2 position;
    vec2 rotation;
    vec2 scale;
} Camera2D;

typedef struct{
    vec3 position __attribute__ ((aligned (16)));
    vec3 rotation __attribute__ ((aligned (16)));
    vec3 scale __attribute__ ((aligned (16)));
} Camera3D;

void Camera2DInit(Camera2D *cam);
void Camera3DInit(Camera3D *cam);

void setActiveCamera(Camera3D *cam);
void setActiveCamera2D(Camera2D *cam);

void setViewRotation(vec3 rotation);
vec3 getViewRotation();

void setViewPos2D(vec2 pos);
void setViewPos(vec3 pos);

vec2 getViewPos2D();
vec3 getViewPos();

void setViewScale2D(vec2 scale);
void setViewScale(vec3 scale);

void destroyCameras();

#endif // CAMERA_H
