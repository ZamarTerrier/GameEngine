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
    Transform2D transform;
} GameObject2D;

void initGameObject2D(GameObject2D* go);

VkVertexInputBindingDescription getBindingDescription();

attrDescr getAttributeDescriptions();

void addTexture(GameObject2D* go, const char* file);

void addUniformObject(localParam* param, VkDeviceSize size);

void createDrawItemsGameObject(GameObject2D* go);

void cleanGameObject(GameObject2D* go);

void destroyGameObject(GameObject2D* go);

void gameObjectDraw(GameObject2D* go);

void updateUniformBuffer(GameObject2D* go);

vec2 getSizeGameObject(GameObject2D* go);

#endif // GAMEOBJECT_H
