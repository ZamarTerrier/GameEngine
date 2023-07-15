#include "primitiveObject.h"

#include <vulkan/vulkan.h>

#include "e_math.h"

#include "e_camera.h"

#include "e_buffer.h"
#include "e_texture.h"
#include "pipeline.h"

#include "lightObject.h"
#include "render_texture.h"

#include "e_blue_print.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_shapes.h"
#include "e_resource_descriptors.h"
#include "e_resource_export.h"

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

    int builded = false;

    vertexParam vParam;
    indexParam iParam;

    po->params = NULL;
    switch(type)
    {
        case ENGINE_PRIMITIVE3D_LINE :
            GraphicsObjectSetVertex(&po->go.graphObj, lineVert, 2, sizeof(Vertex3D), NULL, 0, sizeof(uint32_t));
            break;
        case ENGINE_PRIMITIVE3D_TRIANGLE :
            GraphicsObjectSetVertex(&po->go.graphObj, triVert, 3, sizeof(Vertex3D), triIndx, 3, sizeof(uint32_t));
            break;
        case ENGINE_PRIMITIVE3D_QUAD :
            GraphicsObjectSetVertex(&po->go.graphObj, quadVert, 4, sizeof(Vertex3D), quadIndx, 6, sizeof(uint32_t));
            break;
        case ENGINE_PRIMITIVE3D_PLANE :
            InitPlane3D(&vParam, &iParam, pParam->sectorCount, pParam->stackCount);
            po->params = calloc(1, sizeof(PlaneParam));
            memcpy(po->params, params, sizeof(PlaneParam));
            builded = true;
            break;
        case ENGINE_PRIMITIVE3D_CUBE :
            GraphicsObjectSetVertex(&po->go.graphObj, cubeVert, 24, sizeof(Vertex3D), cubeIndx, 36, sizeof(uint32_t));
            break;
        case ENGINE_PRIMITIVE3D_CUBESPHERE :
            Cubesphere(&vParam, &iParam, csParam->radius, csParam->verperrow);
            po->params = calloc(1, sizeof(CubeSphereParam));
            memcpy(po->params, params, sizeof(CubeSphereParam));
            builded = true;
            break;
        case ENGINE_PRIMITIVE3D_ICOSPHERE :
            IcoSphereGenerator(&vParam, &iParam, sParam->radius);
            po->params = calloc(1, sizeof(SphereParam));
            memcpy(po->params, params, sizeof(SphereParam));
            break;
        case ENGINE_PRIMITIVE3D_SPHERE :
            SphereGenerator3D(&vParam, &iParam, sParam->radius, sParam->sectorCount, sParam->stackCount);
            po->params = calloc(1, sizeof(SphereParam));
            memcpy(po->params, params, sizeof(SphereParam));
            builded = true;
            break;
        case ENGINE_PRIMITIVE3D_CONE :
            ConeGenerator(&vParam, &iParam, cParam->height, cParam->sectorCount, cParam->stackCount);
            po->params = calloc(1, sizeof(ConeParam));
            memcpy(po->params, params, sizeof(ConeParam));
            builded = true;
            break;
        case ENGINE_PRIMITIVE3D_SKYBOX:
            SphereGenerator3D(&vParam, &iParam, sParam->radius, sParam->sectorCount, sParam->stackCount);
            po->params = calloc(1, sizeof(SphereParam));
            memcpy(po->params, params, sizeof(SphereParam));
            builded = true;
            break;
    }

    if(builded)
    {
        GraphicsObjectSetVertex(&po->go.graphObj, vParam.vertices, vParam.verticesSize, sizeof(Vertex3D), iParam.indices, iParam.indexesSize, sizeof(uint32_t));
        free(vParam.vertices);
        free(iParam.indices);
    }

    GameObject3DInitTextures(po, dParam);

    if(type == ENGINE_PRIMITIVE3D_SKYBOX)
        Transform3DSetScale(po, -500, -500, -500);

}

void PrimitiveObjectSetShadowDefaultDescriptor(PrimitiveObject *po, DrawParam *dParam)
{
    uint32_t nums = po->go.graphObj.blueprints.num_blue_print_packs;
    po->go.graphObj.blueprints.blue_print_packs[nums].render_point = dParam->render;

    BluePrintAddUniformObject(&po->go.graphObj.blueprints, nums, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject3DDescriptorModelUpdate, 0);
    BluePrintAddUniformObject(&po->go.graphObj.blueprints, nums, sizeof(DirLightBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DDescriptorDirLightsUpdate, 0);
    BluePrintAddUniformObject(&po->go.graphObj.blueprints, nums, sizeof(PointLightBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DDescriptorPointLightsUpdate, 0);
    BluePrintAddUniformObject(&po->go.graphObj.blueprints, nums, sizeof(SpotLightBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DDescriptorSpotLightsUpdate, 0);
    BluePrintAddUniformObject(&po->go.graphObj.blueprints, nums, sizeof(LightStatusBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DLigtStatusBufferUpdate, 0);

    RenderTexture **renders = dir_shadow_array;

    if(num_dir_shadows > 1)
        BluePrintAddRenderImageArray(&po->go.graphObj.blueprints, nums, renders, num_dir_shadows);
    else
        BluePrintAddRenderImage(&po->go.graphObj.blueprints, nums, renders[0]);

    renders = point_shadow_array;

    if(num_point_shadows > 1)
        BluePrintAddRenderImageArray(&po->go.graphObj.blueprints, nums, renders, num_point_shadows);
    else
        BluePrintAddRenderImage(&po->go.graphObj.blueprints, nums, renders[0]);

    renders = spot_shadow_array;

    if(num_spot_shadows > 1)
        BluePrintAddRenderImageArray(&po->go.graphObj.blueprints, nums, renders, num_spot_shadows);
    else
        BluePrintAddRenderImage(&po->go.graphObj.blueprints, nums, renders[0]);

    BluePrintAddTextureImage(&po->go.graphObj.blueprints, nums, &po->go.images[0], VK_SHADER_STAGE_FRAGMENT_BIT);
    BluePrintAddTextureImage(&po->go.graphObj.blueprints, nums, &po->go.images[1], VK_SHADER_STAGE_FRAGMENT_BIT);

    PipelineSetting setting;

    PipelineSettingSetDefault(&po->go.graphObj, &setting);

    PipelineSettingSetShader(&setting, &_binary_shaders_3d_object_shadow_vert_spv_start, (size_t)(&_binary_shaders_3d_object_shadow_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_3d_object_shadow_frag_spv_start, (size_t)(&_binary_shaders_3d_object_shadow_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);

    setting.fromFile = 0;
    setting.vert_indx = 0;

    GameObject3DAddSettingPipeline(po, nums, &setting);

    po->go.graphObj.blueprints.num_blue_print_packs ++;
}

void PrimitiveObjectSetDefaultDescriptor(PrimitiveObject *po, DrawParam *dParam)
{
    uint32_t nums = po->go.graphObj.blueprints.num_blue_print_packs;
    po->go.graphObj.blueprints.blue_print_packs[nums].render_point = dParam->render;

    BluePrintAddUniformObject(&po->go.graphObj.blueprints, nums, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject3DDescriptorModelUpdate, 0);
    BluePrintAddUniformObject(&po->go.graphObj.blueprints, nums, sizeof(DirLightBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DDescriptorDirLightsUpdate, 0);
    BluePrintAddUniformObject(&po->go.graphObj.blueprints, nums, sizeof(PointLightBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DDescriptorPointLightsUpdate, 0);
    BluePrintAddUniformObject(&po->go.graphObj.blueprints, nums, sizeof(SpotLightBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DDescriptorSpotLightsUpdate, 0);
    BluePrintAddUniformObject(&po->go.graphObj.blueprints, nums, sizeof(LightStatusBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DLigtStatusBufferUpdate, 0);

    BluePrintAddTextureImage(&po->go.graphObj.blueprints, nums, &po->go.images[0], VK_SHADER_STAGE_FRAGMENT_BIT);
    BluePrintAddTextureImage(&po->go.graphObj.blueprints, nums, &po->go.images[1], VK_SHADER_STAGE_FRAGMENT_BIT);

    PipelineSetting setting;

    PipelineSettingSetDefault(&po->go.graphObj, &setting);

    PipelineSettingSetShader(&setting, &_binary_shaders_3d_object_vert_spv_start, (size_t)(&_binary_shaders_3d_object_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_3d_object_frag_spv_start, (size_t)(&_binary_shaders_3d_object_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);

    setting.fromFile = 0;
    setting.vert_indx = 0;

    GameObject3DAddSettingPipeline(po, nums, &setting);

    po->go.graphObj.blueprints.num_blue_print_packs ++;
}

void PrimitiveObjectSetInstanceDescriptor(PrimitiveObject *po, DrawParam *dParam)
{
    uint32_t nums = po->go.graphObj.blueprints.num_blue_print_packs;
    po->go.graphObj.blueprints.blue_print_packs[nums].render_point = dParam->render;

    BluePrintAddUniformObject(&po->go.graphObj.blueprints, nums, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject3DDescriptorModelUpdate, 0);

    BluePrintAddTextureImage(&po->go.graphObj.blueprints, nums, &po->go.images[0], VK_SHADER_STAGE_FRAGMENT_BIT);

    PipelineSetting setting;

    PipelineSettingSetDefault(&po->go.graphObj, &setting);

    PipelineSettingSetShader(&setting, &_binary_shaders_3d_object_instance_vert_spv_start, (size_t)(&_binary_shaders_3d_object_instance_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_3d_object_instance_frag_spv_start, (size_t)(&_binary_shaders_3d_object_instance_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);

    setting.fromFile = 0;
    setting.vert_indx = 0;

    GameObject3DAddSettingPipeline(po, nums, &setting);

    po->go.graphObj.blueprints.num_blue_print_packs ++;
}


void *PrimitiveObjectGetVertex(PrimitiveObject *po)
{
    return &po->go.graphObj.shapes[0].vParam;
}

void PrimitiveObjectAddShadow(PrimitiveObject *po, DrawParam *dParam)
{
    RenderTexture **renders = dir_shadow_array;

    for(int i=0;i < num_dir_shadows;i++)
        GameObject3DAddShadowDescriptor(po, ENGINE_LIGHT_TYPE_DIRECTIONAL, renders[i], i);

    renders = point_shadow_array;

    for(int i=0;i < num_point_shadows;i++)
        GameObject3DAddOmiShadow(po, renders[i], i);

    renders = spot_shadow_array;

    for(int i=0;i < num_spot_shadows;i++)
        GameObject3DAddShadowDescriptor(po, ENGINE_LIGHT_TYPE_SPOT, renders[i], i);

    PrimitiveObjectSetShadowDefaultDescriptor(po, dParam);
}

void PrimitiveObjectInitDefault(PrimitiveObject *po, DrawParam *dParam, char type, void *params)
{
        PrimitiveObjectInit(po, dParam, type, params);

        if(dParam->flags & ENGINE_DRAW_PARAM_FLAG_ADD_SHADOW)
            PrimitiveObjectAddShadow(po, dParam);
        else
            PrimitiveObjectSetDefaultDescriptor(po, dParam);

        GameObject3DInitDraw(po);
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
