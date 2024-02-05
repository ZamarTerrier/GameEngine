#include "grass_object.h"

#include <vulkan/vulkan.h>

#include "e_buffer.h"
#include "pipeline.h"

#include "e_tools.h"

#include "e_resource_shapes.h"
#include "e_resource_export.h"

void GrassObjectSetDefaultDescriptor(GrassObject *grass, DrawParam *dParam)
{

    uint32_t nums = grass->go.graphObj.blueprints.num_blue_print_packs;
    grass->go.graphObj.blueprints.blue_print_packs[nums].render_point = dParam->render;

    BluePrintAddUniformObject(&grass->go.graphObj.blueprints, nums, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject3DDescriptorModelUpdate, 0);
    BluePrintAddUniformObject(&grass->go.graphObj.blueprints, nums, sizeof(DirLightBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DDescriptorDirLightsUpdate, 0);
    BluePrintAddUniformObject(&grass->go.graphObj.blueprints, nums, sizeof(PointLightBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DDescriptorPointLightsUpdate, 0);
    BluePrintAddUniformObject(&grass->go.graphObj.blueprints, nums, sizeof(SpotLightBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DDescriptorSpotLightsUpdate, 0);
    BluePrintAddUniformObject(&grass->go.graphObj.blueprints, nums, sizeof(LightStatusBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject3DLigtStatusBufferUpdate, 0);

    BluePrintAddTextureImage(&grass->go.graphObj.blueprints, nums, &grass->go.images[0], VK_SHADER_STAGE_FRAGMENT_BIT);
    BluePrintAddTextureImage(&grass->go.graphObj.blueprints, nums, &grass->go.images[1], VK_SHADER_STAGE_FRAGMENT_BIT);

    PipelineSetting setting;

    PipelineSettingSetDefault(&grass->go.graphObj, &setting);

    PipelineSettingSetShader(&setting, &_binary_shaders_3d_object_vert_spv_start, (size_t)(&_binary_shaders_3d_object_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_3d_object_frag_spv_start, (size_t)(&_binary_shaders_3d_object_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);

    setting.fromFile = 0;
    setting.vert_indx = 0;
    setting.cull_mode = VK_CULL_MODE_NONE;

    GameObject3DAddSettingPipeline(grass, nums, &setting);

    grass->go.graphObj.blueprints.num_blue_print_packs ++;
}

void GrassObjectSetInstanceDescriptor(GrassObject *grass, DrawParam *dParam)
{

    uint32_t nums = grass->go.graphObj.blueprints.num_blue_print_packs;
    grass->go.graphObj.blueprints.blue_print_packs[nums].render_point = dParam->render;

    BluePrintAddUniformObject(&grass->go.graphObj.blueprints, nums, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject3DDescriptorModelUpdate, 0);

    BluePrintAddTextureImage(&grass->go.graphObj.blueprints, nums, &grass->go.images[0], VK_SHADER_STAGE_FRAGMENT_BIT);

    PipelineSetting setting;

    PipelineSettingSetDefault(&grass->go.graphObj, &setting);

    PipelineSettingSetShader(&setting, &_binary_shaders_3d_object_instance_vert_spv_start, (size_t)(&_binary_shaders_3d_object_instance_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_3d_object_instance_frag_spv_start, (size_t)(&_binary_shaders_3d_object_instance_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);

    setting.fromFile = 0;
    setting.vert_indx = 0;
    setting.cull_mode = VK_CULL_MODE_NONE;

    GameObject3DAddSettingPipeline(grass, nums, &setting);

    grass->go.graphObj.blueprints.num_blue_print_packs ++;
}

void GrassObjectInit(GrassObject *grass, DrawParam *dParam)
{
    GameObject3DInit(grass);

    vertexParam vParam;
    indexParam iParam;

    InitGrass3D(&vParam, &iParam);

    GraphicsObjectSetVertex(&grass->go.graphObj, vParam.vertices, vParam.verticesSize, sizeof(Vertex3D), iParam.indices, iParam.indexesSize, sizeof(uint32_t));

    free(vParam.vertices);
    free(iParam.indices);

    GameObject3DInitTextures(grass, dParam);
}
