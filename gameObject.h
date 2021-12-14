#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "stdinclude.h"

#include "graphicsObject.h"
#include "transform.h"

#include "resource.h"
#include "bindDesciption.h"
#include "pipeline.h"
#include "texture.h"

typedef struct{
    GraphicsObject graphObj;
    Transform transform;
} GameObject;

void initGameObject(GameObject* go);

VkVertexInputBindingDescription getBindingDescription();

attrDescr getAttributeDescriptions();

void addTexture(GameObject* go, const char* file);

void addUniformObject(localParam* param, VkDeviceSize size);

void createDrawItemsGameObject(GameObject* go);

void cleanGameObject(GameObject* go);

void destroyGameObject(GameObject* go);

void gameObjectDraw(GameObject* go);

void updateUniformBuffer(GameObject* go);

vec2 getSizeGameObject(GameObject* go);

#endif // GAMEOBJECT_H
