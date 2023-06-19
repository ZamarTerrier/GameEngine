#include "gameObject.h"

#include "engine.h"

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

    if(go == NULL)
        return;

    void (*init)(GameObject* go) = go->InitPoint;

    init(go);
}

void GameObjectUpdate(GameObject* go) {

    if(go == NULL)
        return;

    void (*update)(GameObject* go) = go->UpdatePoint;

    if(update == NULL)
        return;

    update(go);
}

void GameObjectDraw(GameObject* go, void *command) {

    if(go == NULL)
        return;

    void (*draw)(GameObject* go, void *cmd) = go->DrawPoint;

    if(draw == NULL)
        return;

    draw(go, command);
}

void GameObjectClean(GameObject* go){

    if(go == NULL)
        return;

    void (*update)(GameObject* go) = go->CleanPoint;

    update(go);
}

void GameObjectRecreate(GameObject* go){

    if(go == NULL)
        return;

    void (*recreate)(GameObject* go) = go->RecreatePoint;

    recreate(go);
}

void GameObjectDestroy(GameObject* go){

    EngineDeviceWaitIdle();

    if(go == NULL)
        return;

    void (*destroy)(GameObject* go) = go->DestroyPoint;

    destroy(go);
}
