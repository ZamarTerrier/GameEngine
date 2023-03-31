#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "engine_includes.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum{
    ENGINE_GAME_OBJECT_TYPE_2D,
    ENGINE_GAME_OBJECT_TYPE_3D
} GameObjectType;

typedef struct {
    void *InitPoint;
    void *UpdatePoint;
    void *DrawPoint;
    void *CleanPoint;
    void *RecreatePoint;
    void *DestroyPoint;
    uint32_t obj_type;
} GameObject;

void GameObjectSetInitFunc(GameObject *go, void *func);
void GameObjectSetUpdateFunc(GameObject *go, void *func);
void GameObjectSetDrawFunc(GameObject *go, void *func);
void GameObjectSetCleanFunc(GameObject *go, void *func);
void GameObjectSetRecreateFunc(GameObject *go, void *func);
void GameObjectSetDestroyFunc(GameObject *go, void *func);

void GameObjectInit(GameObject* go);
void GameObjectUpdate(GameObject* go) ;
void GameObjectDraw(GameObject* go, void *command);
void GameObjectClean(GameObject* go);
void GameObjectRecreate(GameObject* go);
void GameObjectDestroy(GameObject* go);

#ifdef __cplusplus
}
#endif

#endif // GAMEOBJECT_H
