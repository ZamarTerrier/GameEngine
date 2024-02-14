#ifndef GAMEOBJECT2D_H
#define GAMEOBJECT2D_H

#include "Variabels/engine_includes.h"

#include "Core/e_descriptor.h"
#include "Core/pipeline.h"
#include "Core/e_texture.h"
#include "Core/graphicsObject.h"
#include "Core/e_transform.h"

#include "gameObject.h"


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

void GameObject2DTransformBufferUpdate(GameObject2D *go, BluePrintDescriptor *descriptor);
void GameObject2DImageBuffer(GameObject2D *go, BluePrintDescriptor *descriptor);

void GameObject2DDefaultUpdate(GameObject2D* go);
void GameObject2DDefaultDraw(GameObject2D* go, void *command);

void GameObject2DInitDraw(GameObject2D *go);
void GameObject2DInit(GameObject2D* go);
void GameObject2DClean(GameObject2D* go);
void GameObject2DRecreate(GameObject2D* go);
void GameObject2DDestroy(GameObject2D* go);

void GameObject2DAddSettingPipeline(GameObject2D* go, uint32_t indx_pack, void *arg);

vec2 GameObject2DGetSize(GameObject2D* go);

#ifdef __cplusplus
}
#endif

#endif // GAMEOBJECT2D_H
