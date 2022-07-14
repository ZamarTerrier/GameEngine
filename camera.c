#include "camera.h"

#include "resource.h"

void initCamera(){

    free(camObj);
    camObj = NULL;
    camObj = (Camera*) calloc(1, sizeof(Camera));

    Camera* cam = (Camera*)camObj;

    cam->position.x =(WIDTH / 2) / (WIDTH * 1.5);
    cam->position.y = (HEIGHT / 2) / (HEIGHT * 1.5);
    cam->position.z = 0.0f;

    cam->scale.x = 1;
    cam->scale.y = 1;
    cam->scale.z = 1;


    cam->rotation.x = 0;
    cam->rotation.y = 0;

}

void setViewPos2D(vec2 pos){
    Camera* cam = (Camera*)camObj;
    vec3 tempVec;
    tempVec.x = pos.x;
    tempVec.y = pos.y;
    tempVec.z = 0.0f;
    cam->position = tempVec;
}

void setViewPos(vec3 pos){
    Camera* cam = (Camera*)camObj;

    cam->position = pos;
}

vec2 getViewPos2D(){
    Camera* cam = (Camera*)camObj;

    vec2 tempVec;
    tempVec.x = cam->position.x;
    tempVec.y = cam->position.y;

    return tempVec;
}

vec3 getViewPos(){
    Camera* cam = (Camera*)camObj;

    return cam->position;
}

void setViewScale2D(vec2 scale){
    Camera* cam = (Camera*)camObj;

    vec3 tempVec;
    tempVec.x = scale.x;
    tempVec.y = scale.y;
    tempVec.z = 1;
    cam->scale = tempVec;
}

void setViewScale(vec3 scale){
    Camera* cam = (Camera*)camObj;

    cam->scale = scale;
}
