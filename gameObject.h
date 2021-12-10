#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include "stdinclude.h"
#include "resource.h"
#include "bindDesciption.h"
#include "pipeline.h"
#include "texture.h"

typedef struct{
    localParam local;
    GraphicItems gItems;
    aboutShader aShader;
    Shape shape;
    vec2 pos;
    vec2 scale;
    struct{
        vec2 offset;
        vec2 scale;
    }img;
} GameObject;

GameObject* initGameObject();

VkVertexInputBindingDescription getBindingDescription();

attrDescr getAttributeDescriptions();

void addTexture(GameObject* go, const char* file);

void addUniformObject(localParam* param, VkDeviceSize size);

void cleanGameObject(GameObject* go);

void createDrawningParams( GraphicItems* gi, localParam* param);

void destroyGameObject(GameObject* go);

void gameObjectDraw(GameObject* go);

void recreateDrawningParams(GameObject* go);

void updateUniformBuffer(GameObject* go);

void setPosGameObject(GameObject* go, vec2 pos);

vec2 getPosGameObject(GameObject* go);

void setScaleGameObject(GameObject* go, vec2 scale);

vec2 getScaleGameObject(GameObject* go);

vec2 getSizeGameObject(GameObject* go);

#endif // GAMEOBJECT_H
