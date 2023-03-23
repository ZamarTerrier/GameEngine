#include "sky_object.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "camera.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_descriptors.h"
#include "e_resource_export.h"
#include "e_resource_shapes.h"

#include "e_math.h"

void SkyObjectDefaultUpdate(SkyObject *so)
{
    if(so->go.graphObj.blueprints.descriptors == NULL)
        return;

    Camera3D *cam = cam3D;
    double time;

    time = glfwGetTime();

    SkyBuffer sb;
    sb.time = time;
    sb.cam_pos = cam->position;
    sb.cam_rot = v3_divs(cam->rotation, 2);

    DescriptorUpdate(so->go.graphObj.blueprints.descriptors, 0, &sb, sizeof(sb));
}

void SkyObjectInit(SkyObject *so, DrawParam *dParam, EngineSkyType type)
{
    GameObjectSetUpdateFunc(so, (void *)SkyObjectDefaultUpdate);
    GameObjectSetDrawFunc(so, (void *)GameObject2DDefaultDraw);
    GameObjectSetCleanFunc(so, (void *)GameObject2DClean);
    GameObjectSetRecreateFunc(so, (void *)GameObject2DRecreate);
    GameObjectSetDestroyFunc(so, (void *)GameObject2DDestroy);

    Transform3DInit(&so->go.transform);
    GraphicsObjectInit(&so->go.graphObj, ENGINE_VERTEX_TYPE_SKY);

    so->go.graphObj.shape.linked = false;

    GraphicsObjectSetVertexSize(&so->go.graphObj, sizeof(SkyVertex), sizeof(uint32_t));

    SkyVertex *some_vertex = calloc(4, sizeof(SkyVertex));
    some_vertex[0].position.x = -1;
    some_vertex[0].position.y = -1;
    some_vertex[0].texture_uv.x = 0;
    some_vertex[0].texture_uv.y = 0;
    some_vertex[1].position.x = -1;
    some_vertex[1].position.y = 1;
    some_vertex[1].texture_uv.x = 0;
    some_vertex[1].texture_uv.y = 1.0;
    some_vertex[2].position.x = 1;
    some_vertex[2].position.y = 1;
    some_vertex[2].texture_uv.x = 1;
    some_vertex[2].texture_uv.y = 1;
    some_vertex[3].position.x = 1;
    some_vertex[3].position.y = -1;
    some_vertex[3].texture_uv.x = 1;
    some_vertex[3].texture_uv.y = 0;

    uint32_t *some_index = calloc(6, sizeof(uint32_t));
    memcpy(some_index, projPlaneIndx, sizeof(uint32_t) * 6);

    GraphicsObjectSetVertex(&so->go.graphObj, some_vertex, 4, some_index, 6);

    GameObjectSetUpdateFunc(&so->go, (void *)SkyObjectDefaultUpdate);

    so->go.graphObj.blueprints.count = 0;

    BuffersAddUniformObject(&so->go.graphObj.blueprints, sizeof(SkyBuffer), VK_SHADER_STAGE_FRAGMENT_BIT);

    GraphicsObjectCreateDrawItems(&so->go.graphObj, false);

    PipelineSetting setting;

    PipelineSettingSetDefault(&so->go.graphObj, &setting);

    if(dParam != NULL && strlen(dParam->fragShader) > 0 && strlen(dParam->vertShader) > 0)
    {
        GraphicsObjectSetShadersPath(&so->go.graphObj, dParam->vertShader, dParam->fragShader);

        setting.vertShader = dParam->vertShader;
        setting.sizeVertShader = 0;
        setting.fragShader = dParam->fragShader;
        setting.sizeFragShader = 0;
        setting.topology = dParam->topology;
        setting.drawType = dParam->drawType;
        setting.fromFile = 1;
        GameObject3DAddSettingPipeline(so, &setting);

    }else{

        switch(type){
            case ENGINE_SKY_TYPE_NIGHT:
                setting.vertShader = &_binary_shaders_sky_stars_vert_spv_start;
                setting.sizeVertShader = (size_t)(&_binary_shaders_sky_stars_vert_spv_size);
                setting.fragShader = &_binary_shaders_sky_stars_frag_spv_start;
                setting.sizeFragShader = (size_t)(&_binary_shaders_sky_stars_frag_spv_size);
                break;
            case ENGINE_SKY_TYPE_DAY:
                setting.vertShader = &_binary_shaders_sky_sky_vert_spv_start;
                setting.sizeVertShader = (size_t)(&_binary_shaders_sky_sky_vert_spv_size);
                setting.fragShader = &_binary_shaders_sky_sky_frag_spv_start;
                setting.sizeFragShader = (size_t)(&_binary_shaders_sky_sky_frag_spv_size);
                break;
            case ENGINE_SKY_TYPE_ATMOSPHERIC:
                setting.vertShader = &_binary_shaders_sky_atmospheric_vert_spv_start;
                setting.sizeVertShader = (size_t)(&_binary_shaders_sky_atmospheric_vert_spv_size);
                setting.fragShader = &_binary_shaders_sky_atmospheric_frag_spv_start;
                setting.sizeFragShader = (size_t)(&_binary_shaders_sky_atmospheric_frag_spv_size);
                break;
            default:
                setting.vertShader = &_binary_shaders_sky_atmospheric_vert_spv_start;
                setting.sizeVertShader = (size_t)(&_binary_shaders_sky_atmospheric_vert_spv_size);
                setting.fragShader = &_binary_shaders_sky_atmospheric_frag_spv_start;
                setting.sizeFragShader = (size_t)(&_binary_shaders_sky_atmospheric_frag_spv_size);
                break;
        }



        setting.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        setting.drawType = 0;
        setting.fromFile = 0;
        GameObject3DAddSettingPipeline(so, &setting);

    }

    PipelineCreateGraphics(&so->go.graphObj, false);
}
