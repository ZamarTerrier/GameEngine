#include "primitiveObject.h"

#include <vulkan/vulkan.h>

#include "e_math.h"

#include "camera.h"

#include "buffers.h"
#include "texture.h"
#include "pipeline.h"

#include "e_blue_print.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_shapes.h"
#include "e_resource_descriptors.h"
#include "e_resource_export.h"

void PrimitiveObjectInitTexture(PrimitiveObject *po, DrawParam *dParam)
{
    po->go.images = calloc(3, sizeof(GameObjectImage));

    if(dParam == NULL)
        return;

    if(strlen(dParam->diffuse) != 0)
    {
        int len = strlen(dParam->diffuse);
        po->go.images[0].path = calloc(len + 1, sizeof(char));
        memcpy(po->go.images[0].path, dParam->diffuse, len);
        po->go.images[0].path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }

    if(strlen(dParam->normal) != 0)
    {
        int len = strlen(dParam->normal);
        po->go.images[1].path = calloc(len + 1, sizeof(char));
        memcpy(po->go.images[1].path, dParam->normal, len);
        po->go.images[1].path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }


    if(strlen(dParam->specular) != 0)
    {
        int len = strlen(dParam->specular);
        po->go.images[2].path = calloc(len + 1, sizeof(char));
        memcpy(po->go.images[2].path, dParam->specular, len);
        po->go.images[0].path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }

}

void PrimitiveObjectDestroy(PrimitiveObject *po)
{
    GameObject3DDestroy(po);

    free(po->params);
    po->params = NULL;
}

void PrimitiveObjectInit(PrimitiveObject *po, DrawParam *dParam, char type, void *params){

    GameObject3DInit(po);

    GameObjectSetDestroyFunc(po, (void *)PrimitiveObjectDestroy);

    po->type = type;

    PlaneParam *pParam = (PlaneParam *)params;
    SphereParam *sParam = (SphereParam *)params;
    CubeSphereParam *csParam = (CubeSphereParam *)params;
    ConeParam *cParam = (ConeParam *)params;

    GraphicsObjectSetVertexSize(&po->go.graphObj, 0, sizeof(Vertex3D), sizeof(uint32_t));

    int builded = false;

    po->params = NULL;
    switch(type)
    {
        case ENGINE_PRIMITIVE3D_LINE :
            GraphicsObjectSetVertex(&po->go.graphObj, 0, lineVert, 2, NULL, 0);
            break;
        case ENGINE_PRIMITIVE3D_TRIANGLE :
            GraphicsObjectSetVertex(&po->go.graphObj, 0, triVert, 3, triIndx, 3);
            break;
        case ENGINE_PRIMITIVE3D_QUAD :
            GraphicsObjectSetVertex(&po->go.graphObj, 0, quadVert, 4, quadIndx, 6);
            break;
        case ENGINE_PRIMITIVE3D_PLANE :
            InitPlane3D(&po->go.graphObj.shapes[0].vParam, &po->go.graphObj.shapes[0].iParam, pParam->sectorCount, pParam->stackCount);
            po->params = calloc(1, sizeof(PlaneParam));
            memcpy(po->params, params, sizeof(PlaneParam));
            builded = true;
            break;
        case ENGINE_PRIMITIVE3D_CUBE :
            GraphicsObjectSetVertex(&po->go.graphObj, 0, cubeVert, 24, cubeIndx, 36);
            break;
        case ENGINE_PRIMITIVE3D_CUBESPHERE :
            Cubesphere(&po->go.graphObj.shapes[0].vParam, &po->go.graphObj.shapes[0].iParam, csParam->radius, csParam->verperrow);
            po->params = calloc(1, sizeof(CubeSphereParam));
            memcpy(po->params, params, sizeof(CubeSphereParam));
            builded = true;
            break;
        case ENGINE_PRIMITIVE3D_ICOSPHERE :
            IcoSphereGenerator(&po->go.graphObj.shapes[0].vParam, &po->go.graphObj.shapes[0].iParam, sParam->radius);
            po->params = calloc(1, sizeof(SphereParam));
            memcpy(po->params, params, sizeof(SphereParam));
            break;
        case ENGINE_PRIMITIVE3D_SPHERE :
            SphereGenerator3D(&po->go.graphObj.shapes[0].vParam, &po->go.graphObj.shapes[0].iParam, sParam->radius, sParam->sectorCount, sParam->stackCount);
            po->params = calloc(1, sizeof(SphereParam));
            memcpy(po->params, params, sizeof(SphereParam));
            builded = true;
            break;
        case ENGINE_PRIMITIVE3D_CONE :
            ConeGenerator(&po->go.graphObj.shapes[0].vParam, &po->go.graphObj.shapes[0].iParam, cParam->height, cParam->sectorCount, cParam->stackCount);
            po->params = calloc(1, sizeof(ConeParam));
            memcpy(po->params, params, sizeof(ConeParam));
            builded = true;
            break;
        case ENGINE_PRIMITIVE3D_SKYBOX:
            SphereGenerator3D(&po->go.graphObj.shapes[0].vParam, &po->go.graphObj.shapes[0].iParam, sParam->radius, sParam->sectorCount, sParam->stackCount);
            po->params = calloc(1, sizeof(SphereParam));
            memcpy(po->params, params, sizeof(SphereParam));
            builded = true;
            break;
    }

    if(builded)
    {
        BuffersCreateVertex(&po->go.graphObj.shapes[0].vParam);
        BuffersCreateIndex(&po->go.graphObj.shapes[0].iParam);
        BuffersUpdateVertex(&po->go.graphObj.shapes[0].vParam);
        BuffersUpdateIndex(&po->go.graphObj.shapes[0].iParam);
    }

    po->go.graphObj.num_shapes = 1;

    PrimitiveObjectInitTexture(po, dParam);

    if(type == ENGINE_PRIMITIVE3D_SKYBOX)
        Transform3DSetScale(po, -500, -500, -500);

}

void PrimitiveObjectInitDefault(PrimitiveObject *po, DrawParam *dParam, char type, void *params)
{
        PrimitiveObjectInit(po, dParam, type, params);

        if(dParam->flags & ENGINE_DRAW_PARAM_FLAG_ADD_SHADOW)
            GameObject3DAddShadowDescriptor(po, dParam->shadow);

        PrimitiveObjectAddDefault(po, dParam->render);
        BluePrintAddRenderImage(&po->go.graphObj.blueprints, 1, dParam->shadow);
        GameObject3DInitDraw(po);
}

void PrimitiveObjectAddDefault(PrimitiveObject *po, void *render)
{
    uint32_t nums = po->go.graphObj.blueprints.num_blue_print_packs;
    po->go.graphObj.blueprints.blue_print_packs[nums].render_point = render;

    BluePrintAddUniformObject(&po->go.graphObj.blueprints, nums, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT);
    BluePrintAddUniformObject(&po->go.graphObj.blueprints, nums, sizeof(LightSpaceMatrix), VK_SHADER_STAGE_VERTEX_BIT);
    BluePrintAddUniformObject(&po->go.graphObj.blueprints, nums, sizeof(LightBuffer3D), VK_SHADER_STAGE_FRAGMENT_BIT);

    BluePrintAddTextureImage(&po->go.graphObj.blueprints, nums, &po->go.images[0]);
    BluePrintAddTextureImage(&po->go.graphObj.blueprints, nums, &po->go.images[1]);

    PipelineSetting setting;

    PipelineSettingSetDefault(&po->go.graphObj, &setting);

    setting.vertShader = &_binary_shaders_3d_object_vert_spv_start;
    setting.sizeVertShader = (size_t)(&_binary_shaders_3d_object_vert_spv_size);
    setting.fragShader = &_binary_shaders_3d_object_frag_spv_start;
    setting.sizeFragShader = (size_t)(&_binary_shaders_3d_object_frag_spv_size);
    setting.fromFile = 0;
    setting.vert_indx = 0;

    GameObject3DAddSettingPipeline(po, nums, &setting);

    po->go.graphObj.blueprints.num_blue_print_packs ++;
}

void *PrimitiveObjectGetVertex(PrimitiveObject *po)
{
    return &po->go.graphObj.shapes[0].vParam;
}

//Не корректно
void PrimitiveObjectDiffuseTextureSetData(PrimitiveObject *po, void *data, uint32_t size_data, uint32_t offset)
{
    /*ShaderDescriptor *descriprots = po->go.graphObj.blueprints.descriptors;

    TextureUpdate(&descriprots[2], data, size_data, offset);*/
}

//Не корректно
void PrimitiveObjectDiffuseSetTexture(PrimitiveObject *po, const char *path)
{
    /*ShaderDescriptor *descriprots = po->go.graphObj.blueprints.descriptors;

    TextureSetTexture(&descriprots[2], path);*/
}
