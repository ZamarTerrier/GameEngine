#include "water_object.h"

#include <vulkan/vulkan.h>

#include <GLFW/glfw3.h>

#include "pipeline.h"

#include "e_tools.h"

#include "e_resource_export.h"

void WaterObjectUpdateBuffer(GameObject3D* go, BluePrintDescriptor *descriptor)
{

    double time = glfwGetTime();

    WaterBuffer wb;

    wb.time = time;

    DescriptorUpdate(descriptor, &wb, sizeof(wb));
}


void WaterObjectSetDefaultDescriptor(WaterObject *water, DrawParam *dParam)
{

    uint32_t nums = water->go.graphObj.blueprints.num_blue_print_packs;
    water->go.graphObj.blueprints.blue_print_packs[nums].render_point = dParam->render;

    BluePrintAddUniformObject(&water->go.graphObj.blueprints, nums, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject3DDescriptorModelUpdate, 0);
    BluePrintAddUniformObject(&water->go.graphObj.blueprints, nums, sizeof(WaterBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)WaterObjectUpdateBuffer, 0);

    BluePrintAddTextureImage(&water->go.graphObj.blueprints, nums, &water->go.images[0], VK_SHADER_STAGE_FRAGMENT_BIT);

    PipelineSetting setting;

    PipelineSettingSetDefault(&water->go.graphObj, &setting);

    PipelineSettingSetShader(&setting, &_binary_shaders_water_vert_spv_start, (size_t)(&_binary_shaders_water_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_water_frag_spv_start, (size_t)(&_binary_shaders_water_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);

    setting.fromFile = 0;
    setting.vert_indx = 0;

    GameObject3DAddSettingPipeline(water, nums, &setting);

    water->go.graphObj.blueprints.num_blue_print_packs ++;
}

void WaterObjectInit(WaterObject *water, DrawParam *dParam, uint32_t size){

    GameObject3DInit(water);

    vertexParam vParam;
    indexParam iParam;

    InitPlane3D(&vParam, &iParam, size, size);

    GraphicsObjectSetVertex(&water->go.graphObj, vParam.vertices, vParam.verticesSize, sizeof(Vertex3D), iParam.indices, iParam.indexesSize, sizeof(uint32_t));

    free(vParam.vertices);
    free(iParam.indices);

    GameObject3DInitTextures(water, dParam);
}

void WaterObjectInitDefault(WaterObject *water, DrawParam *dParam, uint32_t size){
    WaterObjectInit(water, dParam, size);
    WaterObjectSetDefaultDescriptor(water, dParam);
    GameObject3DInitDraw(water);
}
