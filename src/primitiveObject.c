#include "primitiveObject.h"

#include <vulkan/vulkan.h>

#include "e_math.h"

#include "camera.h"

#include "e_resource.h"
#include "buffers.h"
#include "texture.h"
#include "pipeline.h"

void PrimitiveObjectInit(GameObject3D *go, DrawParam dParam, char type, void *params){

    GameObject3DInit(go);

    PlaneParam *pParam = (PlaneParam *)params;
    SphereParam *sParam = (SphereParam *)params;
    CubeSphereParam *csParam = (CubeSphereParam *)params;
    ConeParam *cParam = (ConeParam *)params;    
    TerrainParam *tParam = (TerrainParam *)params;

    switch(type)
    {
        case ENGINE_PRIMITIVE3D_LINE :
            GraphicsObjectSetVertex(&go->graphObj, lineVert, 2, NULL, 0);
            break;
        case ENGINE_PRIMITIVE3D_TRIANGLE :
            GraphicsObjectSetVertex(&go->graphObj, triVert, 3, triIndx, 3);
            break;
        case ENGINE_PRIMITIVE3D_QUAD :
            GraphicsObjectSetVertex(&go->graphObj, quadVert, 4, quadIndx, 6);
            break;
        case ENGINE_PRIMITIVE3D_PLANE :
            InitPlane3D(&go->graphObj.shape.vParam, &go->graphObj.shape.iParam, pParam->sectorCount, pParam->stackCount);
            break;
        case ENGINE_PRIMITIVE3D_CUBE :
            Cubesphere(&go->graphObj.shape.vParam, &go->graphObj.shape.iParam, 1, 2);
            break;
        case ENGINE_PRIMITIVE3D_CUBESPHERE :
            Cubesphere(&go->graphObj.shape.vParam, &go->graphObj.shape.iParam, csParam->radius, csParam->verperrow);
            break;
        case ENGINE_PRIMITIVE3D_ICOSPHERE :
            IcoSphereGenerator(&go->graphObj.shape.vParam, &go->graphObj.shape.iParam, sParam->radius);
            break;
        case ENGINE_PRIMITIVE3D_SPHERE :
            SphereGenerator3D(&go->graphObj.shape.vParam, &go->graphObj.shape.iParam, sParam->radius, sParam->sectorCount, sParam->stackCount);
            break;
        case ENGINE_PRIMITIVE3D_CONE :
            ConeGenerator(&go->graphObj.shape.vParam, &go->graphObj.shape.iParam, cParam->height, cParam->sectorCount, cParam->stackCount);
            break;
        case ENGINE_PRIMITIVE3D_SKYBOX:
            SphereGenerator3D(&go->graphObj.shape.vParam, &go->graphObj.shape.iParam, sParam->radius, sParam->sectorCount, sParam->stackCount);
            break;
        case ENGINE_PRIMITIVE3D_TERRAIN:
            InitTerrain3D(&go->graphObj.shape.vParam, &go->graphObj.shape.iParam, tParam->rows, tParam->colmns, tParam->cell_step);
            break;
    }

    GraphicsObjectSetShadersPath(&go->graphObj, dParam.vertShader, dParam.fragShader);

    go->graphObj.local.descrCount = 0;

    BuffersAddUniformObject(&go->graphObj.local, sizeof(ModelBuffer3D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    BuffersAddUniformObject(&go->graphObj.local, sizeof(LightBuffer3D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);

    go->diffuse = calloc(1, sizeof(GameObjectImage));
    go->specular = calloc(1, sizeof(GameObjectImage));
    go->normal = calloc(1, sizeof(GameObjectImage));

    if(strlen(dParam.diffuse) != 0)
    {
        int len = strlen(dParam.diffuse);
        go->diffuse->path = calloc(len + 1, sizeof(char));
        memcpy(go->diffuse->path, dParam.diffuse, len);
        go->diffuse->path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }

    ImageAddTexture(&go->graphObj.local, go->diffuse);

    if(strlen(dParam.specular) != 0)
    {
        int len = strlen(dParam.specular);
        go->specular->path = calloc(len + 1, sizeof(char));
        memcpy(go->specular->path, dParam.specular, len);
        go->specular->path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }

    ImageAddTexture(&go->graphObj.local, go->specular);

    if(strlen(dParam.normal) != 0)
    {
        int len = strlen(dParam.normal);
        go->normal->path = calloc(len + 1, sizeof(char));
        memcpy(go->normal->path, dParam.normal, len);
        go->normal->path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }

    ImageAddTexture(&go->graphObj.local, go->normal);

    GameObject3DCreateDrawItems(&go->graphObj);

    PipelineSetting setting;

    PipelineSettingSetDefault(&go->graphObj, &setting);

    if(strlen(setting.vertShader) == 0 || strlen(setting.fragShader) == 0)
    {
        setting.vertShader = &_binary_shaders_3d_object_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_3d_object_vert_spv_size);
        setting.fragShader = &_binary_shaders_3d_object_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_3d_object_frag_spv_size);
        setting.fromFile = 0;
    }

    GameObject3DAddSettingPipeline(go, &setting);

    PipelineCreateGraphics(&go->graphObj);

    if(type == ENGINE_PRIMITIVE3D_SKYBOX)
        Transform3DSetScale(go, -500, -500, -500);
}
