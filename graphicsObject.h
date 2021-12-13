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

void initGraphicsObject(GraphicsObject* graphObj);

void createDrawningParams(GraphicsObject* graphObj);

void recreateDrawningParams(GraphicsObject* graphObj);

void cleanGraphicsObject(GraphicsObject* graphObj);

void destroyGraphicsObject(GraphicsObject* graphObj);

#endif // GRAPHICSOBJECT_H
