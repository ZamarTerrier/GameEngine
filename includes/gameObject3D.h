#ifndef GAMEOBJECT3D_H
#define GAMEOBJECT3D_H

#include "gameObject.h"
#include "graphicsObject.h"
#include "transform.h"

typedef struct{
    GameObject self;
    GraphicsObject graphObj;
    Transform3D transform;
    GameObjectImage *diffuse;
    GameObjectImage *specular;
    GameObjectImage *normal;
    int enable_light;
    bool wired;
    bool selfCamera;
} GameObject3D;

void GameObject3DDefaultUpdate(GameObject3D* go);
void GameObject3DDefaultDraw(GameObject3D* go);
void GameObject3DClean(GameObject3D* go);
void GameObject3DRecreate(GameObject3D* go);
void GameObject3DDestroy(GameObject3D* go);

void GameObject3DInit(GameObject3D *go);

void GameObject3DEnableLight(GameObject3D *go, bool enable);

void GameObject3DAddSettingPipeline(GameObject3D* go, void *setting);
void GameObject3DCreateDrawItems(GraphicsObject* go);

//Описание вертекса
EIVertexInputBindingDescription GameObject3DGetBindingDescription();

#endif // GAMEOBJECT3D_H
