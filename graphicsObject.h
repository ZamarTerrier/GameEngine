#ifndef GRAPHICSOBJECT_H
#define GRAPHICSOBJECT_H

#include "stdinclude.h"
#include "graphicsItems.h"

typedef struct{
    localParam local;
    GraphicItems gItems;
    aboutShader aShader;
    bool lock;
    Shape shape;
} GraphicsObject;

typedef struct{
    localParam local;
    GraphicItems gItems;
    aboutShader aShader;
    bool lock;
    Shape3D shape;
} GraphicsObject3D;

void GraphicsObject2DInit(GraphicsObject* graphObj);
void GraphicsObject3DInit(GraphicsObject3D* graphObj);

void GraphicsObject2DSetVertex(GraphicsObject* graphObj, Vertex2D *vert, int vertCount, uint32_t *inx, int indxCount);
void GraphicsObject3DSetVertex(GraphicsObject3D* graphObj, Vertex3D *vert, int vertCount, uint32_t *inx, int indxCount);

void GraphicsObjectCreateDrawItems(GraphicsObject3D* graphObj);
void GraphicsObjectCleanPipelines(GraphicsObject *graphObj);
void GraphicsObjectClean(GraphicsObject* graphObj);
void GraphicsObjectDestroy(GraphicsObject* graphObj);

void GraphicsObjectSetShadersPath(GraphicsObject* graphOb, const char* vert, const char* frag);

#endif // GRAPHICSOBJECT_H
