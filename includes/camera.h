#ifndef CAMERA_H
#define CAMERA_H

#include "engine_includes.h"

#ifdef __cplusplus
extern "C"
{
#endif

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

void setViewRotation(float x, float y, float z);
vec3 getViewRotation();

void setViewPos2D(float x, float y);
void setViewPos(float x, float y, float z);

vec2 getViewPos2D();
vec3 getViewPos();

void setViewScale2D(float x, float y);
void setViewScale(float x, float y, float z);

void destroyCameras();

#ifdef __cplusplus
}
#endif

#endif // CAMERA_H
