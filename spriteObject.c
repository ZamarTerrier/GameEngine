#include "spriteObject.h"

#include "stdinclude.h"


void initSpriteObject(GameObject2D *go, vec2 size, vec2 position, const char* texturePath, const char* vertShader, const char* fragShader){

    initGameObject2D(go);

    GraphicsObject2DSetVertex(&go->graphObj, planeVert, 4, planeIndx, 6);

    SetShadersPath(&go->graphObj, vertShader, fragShader);
    addUniformObject(&go->graphObj.local, sizeof(ViewBuffer2D));
    addUniformObject(&go->graphObj.local, sizeof(TransformBuffer2D));
    addUniformObject(&go->graphObj.local, sizeof(ImgBuffer));

    addTexture(go, texturePath);
    createDrawItemsGameObject(&go->graphObj);

    vec2 tempVec;
    tempVec.x = position.x;
    tempVec.y = position.y;

    setPosTransform2D(go, tempVec);

    tempVec.x = size.x;
    tempVec.y = size.y;
    setScaleTransform2D(go, tempVec);
}
