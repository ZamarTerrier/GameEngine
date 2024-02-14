#ifndef E_RENDER_OBJECT_H
#define E_RENDER_OBJECT_H

#include "Variabels/engine_includes.h"

#include "render_texture.h"

typedef enum{
    ENGINE_RENDER_OBJECT_MAIN,
} RenderObjectType;

typedef struct{
    RenderTexture render;
    RenderObjectType type;
} RenderObject;

#endif // E_RENDER_OBJECT_H
