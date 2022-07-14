#ifndef GAMEOBJECT3D_H
#define GAMEOBJECT3D_H

#include "graphicsObject.h"
#include "transform.h"

typedef struct{
    GraphicsObject3D graphObj;
    Transform3D transform;
} GameObject3D;

void initGameObject3D(GameObject3D *go);

void GameObject3DAddTexture(GameObject3D* go, const char* file);

void GameObject3DChangeTexture(GameObject3D* go, int elem, const char* file);

void GameObject3DAddUniformObject(localParam* param, VkDeviceSize size);

void GameObject3DCreateDrawItems(GameObject3D* go);

void GameObject3DClean(GameObject3D* go);

//Описание вертекса
VkVertexInputBindingDescription GameObject3DGetBindingDescription();

void GameObject3DDestroy(GameObject3D* go);

void GameObject3DDraw(GameObject3D* go);

void GameObject3DUpdateUniformBuffer(GameObject3D* go) ;

#endif // GAMEOBJECT3D_H
