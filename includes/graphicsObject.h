#ifndef GRAPHICSOBJECT_H
#define GRAPHICSOBJECT_H

#include "engine_includes.h"
#include "graphicsItems.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    Blueprints blueprints;
    GraphicItems gItems;
    aboutShader aShader;
    bool lock;
    Shape shape;
} GraphicsObject;

void GraphicsObjectInit(GraphicsObject* graphObj, uint32_t type);

void GraphicsObjectSetVertexSize(GraphicsObject* graphObj, uint32_t type_v_size, uint32_t type_i_size);
void GraphicsObjectSetVertex(GraphicsObject* graphObj, void *vert, int vertCount, uint32_t *inx, int indxCount);

void GraphicsObjectCreateDrawItems(GraphicsObject* graphObj, bool with_shadow);
void GraphicsObjectCleanPipelines(GraphicsObject *graphObj);
void GraphicsObjectClean(GraphicsObject* graphObj);
void GraphicsObjectDestroy(GraphicsObject* graphObj);

void GraphicsObjectSetShadersPath(GraphicsObject* graphOb, const char* vert, const char* frag);

#ifdef __cplusplus
}
#endif

#endif // GRAPHICSOBJECT_H
