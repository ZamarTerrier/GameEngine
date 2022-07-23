#include "primitiveObject.h"

#include "resource.h"

void PrimitiveObjectInit(GameObject3D *go, vec3 size, vec3 position, const char* texturePath, const char* vertShader, const char* fragShader, char type){

    initGameObject3D(go);

    switch(type)
    {
        case ENGINE_PRIMITIVE3D_TRIANGLE :
            GraphicsObject3DSetVertex(&go->graphObj, triVert, 3, cubeIndx, 3);
            break;
        case ENGINE_PRIMITIVE3D_QUAD :
            GraphicsObject3DSetVertex(&go->graphObj, quadVert, 4, quadIndx, 6);
            break;
        case ENGINE_PRIMITIVE3D_CUBE :
            GraphicsObject3DSetVertex(&go->graphObj, cubeVert, 8, cubeIndx, 36);
            break;
    }

    SetShadersPath(&go->graphObj, vertShader, fragShader);

    addUniformObject(&go->graphObj.local, sizeof(ViewBuffer3D));
    addUniformObject(&go->graphObj.local, sizeof(TransformBuffer3D));
    addUniformObject(&go->graphObj.local, sizeof(ModelBuffer3D));

    GameObject3DAddTexture(go, texturePath);
    GameObject3DCreateDrawItems(go);

    setScaleTransform3D(go, size);
    setPosTransform3D(go, position);
}
