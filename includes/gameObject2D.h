#ifndef GAMEOBJECT2D_H
#define GAMEOBJECT2D_H

#include "engine_includes.h"

#include "gameObject.h"
#include "graphicsObject.h"
#include "transform.h"

#include "bindDesciption.h"
#include "pipeline.h"
#include "texture.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    GameObject self;
    GraphicsObject graphObj;
    Transform2D transform;
    GameObjectImage *image;
    char name[256];
} GameObject2D;

void GameObject2DSetLinkedShape(GameObject2D *go);

void GameObject2DDefaultDraw(GameObject2D* go);
void GameObject2DInit(GameObject2D* go);
void GameObject2DClean(GameObject2D* go);
void GameObject2DRecreate(GameObject2D* go);
void GameObject2DDestroy(GameObject2D* go);

void GameObject2DAddSettingPipeline(GameObject2D* go, void *setting);

vec2 GameObject2DGetSize(GameObject2D* go);

#ifdef __cplusplus
}
#endif

#endif // GAMEOBJECT2D_H
