#ifndef SPRITEOBJECT_H
#define SPRITEOBJECT_H

#include "gameObject2D.h"

typedef struct{
    vec2 size;
    vec2 position;
    vec3 color;
    char texturePath[256];
    char vertShader[256];
    char fragShader[256];
} SpriteParam;

typedef struct{
    GameObject2D go;
} SpriteObject;

void SpriteObjectInit(SpriteObject *go, SpriteParam sParam);

#endif // SPRITEOBJECT_H
