#ifndef GRAPHICSOBJECT_H
#define GRAPHICSOBJECT_H

#include "stdinclude.h"
#include "graphicsItems.h"

typedef struct{
    localParam local;
    GraphicItems gItems;
    aboutShader aShader;
    Shape shape;
} GraphicsObject;

typedef struct{
    localParam local;
    GraphicItems gItems;
    aboutShader aShader;
    Shape3D shape;
} GraphicsObject3D;

void initGraphicsObject(GraphicsObject* graphObj);
void initGraphicsObject3D(GraphicsObject3D* graphObj);

void GraphicsObject2DSetVertex(GraphicsObject* graphObj, Vertex2D *vert, int vertCount, uint16_t *inx, int indxCount);
void GraphicsObject3DSetVertex(GraphicsObject3D* graphObj, Vertex3D *vert, int vertCount, uint16_t *inx, int indxCount);

void SetShadersPath(GraphicsObject* graphOb, const char* vert, const char* frag);

void recreateDrawningParams(GraphicsObject* graphObj);

void cleanPipelines(GraphicsObject *graphObj);
void cleanGraphicsObject(GraphicsObject* graphObj);

void destroyGraphicsObject(GraphicsObject* graphObj);

#endif // GRAPHICSOBJECT_H
