#include "camera.h"

#include "e_resource_data.h"

void Camera2DInit(Camera2D *cam){

    memset(cam, 0, sizeof(Camera2D));

    cam->scale.x = 1;
    cam->scale.y = 1;

    Camera2DSetActive(cam);
}

void Camera3DInit(Camera3D *cam){

    memset(cam, 0, sizeof(Camera3D));

    cam->scale.x = 1;
    cam->scale.y = 1;
    cam->scale.z = 1;
    cam->view_distance = 500;
    cam->view_angle = 75.0f;
    cam->view_near = 0.01f;

    Camera3DSetActive(cam);
}

void Camera3DSetViewDistance(Camera3D *cam, double distance)
{
    cam->view_distance = distance;
}

void Camera3DSetViewAngle(Camera3D *cam, double angle)
{
    cam->view_angle = angle;
}

void Camera3DSetViewNear(Camera3D *cam, double near)
{
    cam->view_near = near;
}

void Camera3DSetActive(Camera3D *cam){
    cam3D = cam;
}

void Camera2DSetActive(Camera2D *cam){
    cam2D = cam;
}

void Camera3DSetRotation(float x, float y, float z){
    Camera3D* cam = (Camera3D*)cam3D;

    cam->rotation.x = x;
    cam->rotation.y = y;
    cam->rotation.z = z;
}

vec3 Camera3DGetRotation(){
    Camera3D* cam = (Camera3D*)cam3D;

    return cam->rotation;
}

void Camera2DSetPosition(float x, float y){
    Camera2D* cam = (Camera2D*)cam2D;

    cam->position.x = x;
    cam->position.y = y;
}

void Camera3DSetPosition(float x, float y, float z){
    Camera3D* cam = (Camera3D*)cam3D;

    cam->position.x = x;
    cam->position.y = y;
    cam->position.z = z;
}

vec2 Camera2DGetPosition(){
    Camera2D* cam = (Camera2D*)cam2D;

    return cam->position;
}

vec3 Camera3DGetPosition(){
    Camera3D* cam = (Camera3D*)cam3D;

    return cam->position;
}

void Camera2DSetScale(float x, float y){
    Camera2D* cam = (Camera2D*)cam2D;

    cam->scale.x = x;
    cam->scale.y = y;
}

void Camera3DSetScale(float x, float y, float z){
    Camera3D* cam = (Camera3D*)cam3D;

    cam->scale.x = x;
    cam->scale.y = y;
    cam->scale.z = z;
}
