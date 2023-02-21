#include "projectionPlaneObject.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "e_math.h"

#include "tools.h"

#include "camera.h"

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

    vkMapMemory(e_device, go->graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(pdf), 0, &data);
    memcpy(data, &pdf, sizeof(pdf));
    vkUnmapMemory(e_device,  go->graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex]);

}

void ProjectionPlaneInit(GameObject2D *go, DrawParam dParam){

    GameObject2DInit(go);

    GraphicsObjectSetVertexSize(&go->graphObj, sizeof(Vertex2D), sizeof(uint32_t));
    GraphicsObjectSetVertex(&go->graphObj, projPlaneVert, 4, projPlaneIndx, 6);

    GameObject2DSetLinkedShape(go);

    GameObjectSetUpdateFunc(go, (void *)ProjectionPlaneUpdate);

    GraphicsObjectSetShadersPath(&go->graphObj, dParam.vertShader, dParam.fragShader);

    BuffersAddUniformObject(&go->graphObj.local, sizeof(ProjDataBuffer), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);

    GraphicsObjectCreateDrawItems(&go->graphObj);

    PipelineSetting setting = {};

    PipelineSettingSetDefault(&go->graphObj, &setting);

    if(strlen(setting.vertShader) == 0 || strlen(setting.fragShader) == 0)
    {
        setting.obj_type = ENGINE_TYPE_PROJECT_PLANE;
        setting.vertShader = &_binary_shaders_3d_object_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_3d_object_vert_spv_size);
        setting.fragShader = &_binary_shaders_3d_object_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_3d_object_frag_spv_size);
        setting.fromFile = 0;
    }

    GameObject2DAddSettingPipeline(go, &setting);

    PipelineCreateGraphics(&go->graphObj);
}
