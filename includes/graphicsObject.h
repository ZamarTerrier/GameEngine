#ifndef GRAPHICSOBJECT_H
#define GRAPHICSOBJECT_H

#include "engine_includes.h"
#include "graphicsItems.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    localParam local;
    GraphicItems gItems;
    aboutShader aShader;
    bool lock;
    Shape shape;
} GraphicsObject;

void GraphicsObjectInit(GraphicsObject* graphObj, uint32_t type);

void GraphicsObjectSetVertex(GraphicsObject* graphObj, void *vert, int vertCount, uint32_t *inx, int indxCount);

void GraphicsObjectCreateDrawItems(GraphicsObject* graphObj);
void GraphicsObjectCleanPipelines(GraphicsObject *graphObj);
void GraphicsObjectClean(GraphicsObject* graphObj);
void GraphicsObjectDestroy(GraphicsObject* graphObj);

void GraphicsObjectSetShadersPath(GraphicsObject* graphOb, const char* vert, const char* frag);

#ifdef __cplusplus
}
#endif

#endif // GRAPHICSOBJECT_H
