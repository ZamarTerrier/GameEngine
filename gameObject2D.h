#ifndef GAMEOBJECT2D_H
#define GAMEOBJECT2D_H

#include "stdinclude.h"

#include "gameObject.h"
#include "graphicsObject.h"
#include "transform.h"

#include "e_resource.h"
#include "bindDesciption.h"
#include "pipeline.h"
#include "texture.h"

typedef struct{
    GameObject self;
    GraphicsObject graphObj;
    Transform2D transform;
    GameObjectImage *image;
} GameObject2D;

void GameObject2DInit(GameObject2D* go);

void GameObject2DAddSettingPipeline(GameObject2D* go, PipelineSetting setting);

void GameObject2DCreateDrawItems(GameObject2D* go);

VkVertexInputBindingDescription GameObject2DGetBindingDescription();

vec2 GameObject2DGetSize(GameObject2D* go);

#endif // GAMEOBJECT2D_H
