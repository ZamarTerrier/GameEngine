#ifndef GRAPHICSOBJECT_H
#define GRAPHICSOBJECT_H

#include "engine_includes.h"
#include "graphicsItems.h"

typedef struct{
    localParam local;
    GraphicItems gItems;
    aboutShader aShader;
    bool lock;
    Shape shape;
} GraphicsObject;

void GraphicsObject2DInit(GraphicsObject* graphObj);
void GraphicsObject3DInit(GraphicsObject* graphObj);
void GraphicsObjectModel3DInit(GraphicsObject* graphObj);
void GraphicsObjectParticle3DInit(GraphicsObject* graphObj);

void GraphicsObjectSetVertex(GraphicsObject* graphObj, void *vert, int vertCount, uint32_t *inx, int indxCount);

void GraphicsObjectCreateDrawItems(GraphicsObject* graphObj);
void GraphicsObjectCleanPipelines(GraphicsObject *graphObj);
void GraphicsObjectClean(GraphicsObject* graphObj);
void GraphicsObjectDestroy(GraphicsObject* graphObj);

void GraphicsObjectSetShadersPath(GraphicsObject* graphOb, const char* vert, const char* frag);

#endif // GRAPHICSOBJECT_H
