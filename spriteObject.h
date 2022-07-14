#ifndef SPRITEOBJECT_H
#define SPRITEOBJECT_H

#include "gameObject.h"

typedef struct{
    GameObject2D go;
} SpriteObject;

void initSpriteObject(GameObject2D *go, vec2 size, vec2 position, const char* texturePath, const char* vertShader, const char* fragShader);

#endif // SPRITEOBJECT_H
