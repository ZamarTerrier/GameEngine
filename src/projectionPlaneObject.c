#include "projectionPlaneObject.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "e_math.h"

#include "e_tools.h"

#include "e_camera.h"

#include "e_blue_print.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_shapes.h"
#include "e_resource_export.h"

void ProjectionPlaneUpdate(GameObject2D *go, BluePrintDescriptor *descriptor){

    Camera3D* cam = (Camera3D*) cam3D;

    ProjDataBuffer pdf = {};
    pdf.time = glfwGetTime();
    pdf.camRot = cam->rotation;
    pdf.camPos = v3_divs(cam->position, 10);

    DescriptorUpdate(descriptor, &pdf, sizeof(pdf));
}

void ProjectionPlaneInit(GameObject2D *go, DrawParam dParam){

    GameObject2DInit(go);

    GraphicsObjectSetVertex(&go->graphObj, projPlaneVert, 4, sizeof(Vertex2D), projPlaneIndx, 6, sizeof(uint32_t));

    GraphicsObjectSetShadersPath(&go->graphObj, dParam.vertShader, dParam.fragShader);

}

void ProjectionPlaneAddDefault(GameObject2D *go, void *render)
{
    uint32_t nums = go->graphObj.blueprints.num_blue_print_packs;
    go->graphObj.blueprints.blue_print_packs[nums].render_point = render;

    BluePrintAddUniformObject(&go->graphObj.blueprints, nums, sizeof(ProjDataBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)ProjectionPlaneUpdate, 0);

    PipelineSetting setting;

    PipelineSettingSetDefault(&go->graphObj, &setting);

    PipelineSettingSetShader(&setting, &_binary_shaders_sprite_vert_spv_start, (size_t)(&_binary_shaders_sprite_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_sprite_frag_spv_start, (size_t)(&_binary_shaders_sprite_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);

    setting.fromFile = 0;
    setting.vert_indx = 0;

    GameObject2DAddSettingPipeline(go, nums, &setting);

    go->graphObj.blueprints.num_blue_print_packs ++;
}
