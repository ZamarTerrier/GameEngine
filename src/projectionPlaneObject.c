#include "projectionPlaneObject.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "e_math.h"

#include "tools.h"

#include "camera.h"

#include "e_blue_print.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_shapes.h"
#include "e_resource_export.h"

void ProjectionPlaneUpdate(GameObject2D *go){

    Camera3D* cam = (Camera3D*) cam3D;
    void* data;

    ProjDataBuffer pdf = {};
    pdf.time = glfwGetTime();
    pdf.camRot = cam->rotation;
    pdf.camPos = v3_divs(cam->position, 10);

    DescriptorUpdate(&go->graphObj.blueprints, 0, 0, &pdf, sizeof(pdf));
}

void ProjectionPlaneInit(GameObject2D *go, DrawParam dParam){

    GameObject2DInit(go);

    GraphicsObjectSetVertexSize(&go->graphObj, 0, sizeof(Vertex2D), sizeof(uint32_t));
    GraphicsObjectSetVertex(&go->graphObj, 0, projPlaneVert, 4, projPlaneIndx, 6);

    GameObjectSetUpdateFunc(go, (void *)ProjectionPlaneUpdate);

    GraphicsObjectSetShadersPath(&go->graphObj, dParam.vertShader, dParam.fragShader);

}

void ProjectionPlaneAddDefault(GameObject2D *go, void *render)
{
    uint32_t nums = go->graphObj.blueprints.num_blue_print_packs;
    go->graphObj.blueprints.blue_print_packs[nums].render_point = render;

    BluePrintAddUniformObject(&go->graphObj.blueprints, nums, sizeof(ProjDataBuffer), VK_SHADER_STAGE_FRAGMENT_BIT);

    PipelineSetting setting;

    PipelineSettingSetDefault(&go->graphObj, &setting);

    setting.vertShader = &_binary_shaders_sprite_vert_spv_start;
    setting.sizeVertShader = (size_t)(&_binary_shaders_sprite_vert_spv_size);
    setting.fragShader = &_binary_shaders_sprite_frag_spv_start;
    setting.sizeFragShader = (size_t)(&_binary_shaders_sprite_frag_spv_size);
    setting.fromFile = 0;
    setting.vert_indx = 0;

    GameObject2DAddSettingPipeline(go, nums, &setting);

    go->graphObj.blueprints.num_blue_print_packs ++;
}
