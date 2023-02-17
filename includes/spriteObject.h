#ifndef SPRITEOBJECT_H
#define SPRITEOBJECT_H

#include "gameObject2D.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    vec3 color;
    char texturePath[256];
    char vertShader[256];
    char fragShader[256];
} SpriteParam;

typedef struct{
    GameObject2D go;
} SpriteObject;

void SpriteObjectInit(SpriteObject *go, SpriteParam sParam);

#ifdef __cplusplus
}
#endif

#endif // SPRITEOBJECT_H
