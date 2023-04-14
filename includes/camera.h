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
    double view_distance;
    double view_angle;
    double view_near;
} Camera3D;

void Camera2DInit(Camera2D *cam);
void Camera3DInit(Camera3D *cam);

void Camera3DSetViewDistance(Camera3D *cam, double distance);
void Camera3DSetViewAngle(Camera3D *cam, double angle);
void Camera3DSetViewNear(Camera3D *cam, double near);

void Camera3DSetActive(Camera3D *cam);
void Camera2DSetActive(Camera2D *cam);

void Camera3DSetRotation(float x, float y, float z);
vec3 Camera3DGetRotation();

void Camera2DSetPosition(float x, float y);
void Camera3DSetPosition(float x, float y, float z);

vec2 Camera2DGetPosition();
vec3 Camera3DGetPosition();

void Camera2DSetScale(float x, float y);
void Camera3DSetScale(float x, float y, float z);

void destroyCameras();

#ifdef __cplusplus
}
#endif

#endif // CAMERA_H
