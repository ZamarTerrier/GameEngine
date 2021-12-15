#include "camera.h"

#include "resource.h"

void initCamera(){

    free(camObj);
    camObj = NULL;
    camObj = (Camera*) calloc(1, sizeof(Camera));

    Camera* cam = (Camera*)camObj;

    cam->pos.x =(WIDTH / 2) / (WIDTH * 1.5);
    cam->pos.y = (HEIGHT / 2) / (HEIGHT * 1.5);
    cam->scale.x = 1;
    cam->scale.y = 1;

}

void setViewPos(vec2 pos){
    Camera* cam = (Camera*)camObj;

    cam->pos = pos;
}

vec2 getViewPos(){
    Camera* cam = (Camera*)camObj;

    return cam->pos;
}

void setViewScale(vec2 scale){
    Camera* cam = (Camera*)camObj;

    cam->scale = scale;
}
