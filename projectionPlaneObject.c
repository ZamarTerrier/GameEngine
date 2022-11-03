#include "projectionPlaneObject.h"

#include <GLFW/glfw3.h>

#include "e_math.h"
#include "e_resource.h"

#include "tools.h"

#include "camera.h"

void ProjectionPlaneUpdate(GameObject2D *go){

    Camera3D* cam = (Camera3D*) cam3D;
    void* data;

    ProjDataBuffer pdf = {};
    pdf.time = glfwGetTime();
    pdf.camRot = cam->rotation;
    pdf.camPos = v3_divs(cam->position, 10);

    vkMapMemory(device, go->graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(pdf), 0, &data);
    memcpy(data, &pdf, sizeof(pdf));
    vkUnmapMemory(device,  go->graphObj.local.descriptors[0].uniform->uniformBuffersMemory[imageIndex]);

}

void ProjectionPlaneInit(GameObject2D *go, DrawParam dParam){

    GameObject2DInit(go);

    GraphicsObjectSetVertex(&go->graphObj, projPlaneVert, 4, projPlaneIndx, 6);

    GameObjectSetUpdateFunc(go, (void *)ProjectionPlaneUpdate);

    GraphicsObjectSetShadersPath(&go->graphObj, dParam.vertShader, dParam.fragShader);

    BuffersAddUniformObject(&go->graphObj.local, sizeof(ProjDataBuffer), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);

    GameObject2DCreateDrawItems(go);

    PipelineSetting setting = {};

    PipelineSettingSetDefault(&go->graphObj, &setting);

    if(strlen(setting.vertShader) == 0 || strlen(setting.fragShader) == 0)
    {
        setting.vertShader = &_binary_shaders_3d_object_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_3d_object_vert_spv_size);
        setting.fragShader = &_binary_shaders_3d_object_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_3d_object_frag_spv_size);
        setting.fromFile = 0;
    }

    GameObject2DAddSettingPipeline(go, setting);

    PipelineCreateGraphics(&go->graphObj);
}
