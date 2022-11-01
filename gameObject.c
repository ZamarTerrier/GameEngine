#include "gameObject.h"

void GameObjectSetInitFunc(GameObject *go, void *func){
    go->InitPoint = func;
}

void GameObjectSetUpdateFunc(GameObject *go, void *func){
    go->UpdatePoint = func;
}

void GameObjectSetDrawFunc(GameObject *go, void *func){
    go->DrawPoint = func;
}

void GameObjectSetCleanFunc(GameObject *go, void *func){
    go->CleanPoint = func;
}

void GameObjectSetRecreateFunc(GameObject *go, void *func){
    go->RecreatePoint = func;
}

void GameObjectSetDestroyFunc(GameObject *go, void *func){
    go->DestroyPoint = func;
}


void GameObjectInit(GameObject* go){

    void (*init)(GameObject* go) = go->InitPoint;

    init(go);
}

void GameObjectUpdate(GameObject* go) {

    void (*update)(GameObject* go) = go->UpdatePoint;

    update(go);
}

void GameObjectDraw(GameObject* go) {

    void (*draw)(GameObject* go) = go->DrawPoint;

    draw(go);
}

void GameObjectClean(GameObject* go){

    void (*update)(GameObject* go) = go->CleanPoint;

    update(go);
}

void GameObjectRecreate(GameObject* go){

    void (*recreate)(GameObject* go) = go->RecreatePoint;

    recreate(go);
}

void GameObjectDestroy(GameObject* go){

    void (*destroy)(GameObject* go) = go->DestroyPoint;

    destroy(go);
}
