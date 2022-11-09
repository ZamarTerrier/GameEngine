#include "camera.h"

#include "e_resource.h"

void Camera2DInit(Camera2D *cam){

    cam->position.x = 0;
    cam->position.y = 0;

    cam->scale.x = 1;
    cam->scale.y = 1;

    memset(&cam->rotation, 0, sizeof(vec2));

    setActiveCamera2D(cam);
}

void Camera3DInit(Camera3D *cam){

    memset(&cam->position, 0, sizeof(vec3));

    cam->scale.x = 1;
    cam->scale.y = 1;
    cam->scale.z = 1;

    memset(&cam->rotation, 0, sizeof(vec3));

    setActiveCamera(cam);
}

void setActiveCamera(Camera3D *cam){
    cam3D = cam;
}

void setActiveCamera2D(Camera2D *cam){
    cam2D = cam;
}

void setViewRotation(vec3 rotation){
    Camera3D* cam = (Camera3D*)cam3D;

    cam->rotation = rotation;
}

vec3 getViewRotation(){
    Camera3D* cam = (Camera3D*)cam3D;

    return cam->rotation;
}

void setViewPos2D(vec2 pos){
    Camera2D* cam = (Camera2D*)cam2D;

    cam->position = pos;
}

void setViewPos(vec3 pos){
    Camera3D* cam = (Camera3D*)cam3D;

    cam->position = pos;
}

vec2 getViewPos2D(){
    Camera2D* cam = (Camera2D*)cam2D;

    return cam->position;
}

vec3 getViewPos(){
    Camera3D* cam = (Camera3D*)cam3D;

    return cam->position;
}

void setViewScale2D(vec2 scale){
    Camera2D* cam = (Camera2D*)cam2D;

    cam->scale = scale;
}

void setViewScale(vec3 scale){
    Camera3D* cam = (Camera3D*)cam3D;

    cam->scale = scale;
}
