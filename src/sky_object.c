#include "sky_object.h"

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include "e_camera.h"

#include "lightObject.h"

#include "render_texture.h"

#include "e_blue_print.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_descriptors.h"
#include "e_resource_export.h"
#include "e_resource_shapes.h"

#include "e_math.h"

void SkyObjectSkyBufferUpdate(SkyObject *so, BluePrintDescriptor *descriptor)
{
    Camera3D *cam = cam3D;

    double time = glfwGetTime();

    SkyBuffer sb;
    sb.time = time;
    sb.cam_pos = cam->position;
    sb.cam_rot = v3_divs(cam->rotation, 2);

    sb.dir_light.x = 0.5f;
    sb.dir_light.y = 0.3f;
    sb.dir_light.z = 0;

    if(e_var_num_lights > 0)
    {
        LightObject **lights = e_var_lights;

        for(int i=0;i < e_var_num_lights; i++)
        {
            if(lights[i]->type == ENGINE_LIGHT_TYPE_DIRECTIONAL)
                sb.dir_light = lights[i]->direction;
        }
    }

    DescriptorUpdate(descriptor, &sb, sizeof(sb));
}

void SkyObjectInit(SkyObject *so, DrawParam *dParam, EngineSkyType type)
{
    memset(so, 0, sizeof(SkyObject));

    GameObjectSetUpdateFunc(so, (void *)GameObject2DDefaultUpdate);
    GameObjectSetDrawFunc(so, (void *)GameObject2DDefaultDraw);
    GameObjectSetCleanFunc(so, (void *)GameObject2DClean);
    GameObjectSetRecreateFunc(so, (void *)GameObject2DRecreate);
    GameObjectSetDestroyFunc(so, (void *)GameObject2DDestroy);

    Transform3DInit(&so->go.transform);
    GraphicsObjectInit(&so->go.graphObj, ENGINE_VERTEX_TYPE_SKY);

    so->type = type;

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

    GraphicsObjectSetVertex(&so->go.graphObj, some_vertex, 4, sizeof(SkyVertex), projPlaneIndx, 6, sizeof(uint32_t));

    free(some_vertex);

    so->go.graphObj.num_shapes = 1;
}

void SkyObjectAddDefault(SkyObject *so, void *render)
{
    uint32_t nums = so->go.graphObj.blueprints.num_blue_print_packs;
    so->go.graphObj.blueprints.blue_print_packs[nums].render_point = render;

    BluePrintAddUniformObject(&so->go.graphObj.blueprints, nums, sizeof(SkyBuffer), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)SkyObjectSkyBufferUpdate, 0);

    PipelineSetting setting;

    PipelineSettingSetDefault(&so->go.graphObj, &setting);

    switch(so->type){
        case ENGINE_SKY_TYPE_NIGHT:
            PipelineSettingSetShader(&setting, &_binary_shaders_sky_stars_vert_spv_start, (size_t)(&_binary_shaders_sky_stars_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
            PipelineSettingSetShader(&setting, &_binary_shaders_sky_stars_frag_spv_start, (size_t)(&_binary_shaders_sky_stars_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);
            break;
        case ENGINE_SKY_TYPE_DAY:
            PipelineSettingSetShader(&setting, &_binary_shaders_sky_sky_vert_spv_start, (size_t)(&_binary_shaders_sky_sky_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
            PipelineSettingSetShader(&setting, &_binary_shaders_sky_sky_frag_spv_start, (size_t)(&_binary_shaders_sky_sky_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);
            break;
        case ENGINE_SKY_TYPE_ATMOSPHERIC:
            PipelineSettingSetShader(&setting, &_binary_shaders_sky_atmospheric_vert_spv_start, (size_t)(&_binary_shaders_sky_atmospheric_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
            PipelineSettingSetShader(&setting, &_binary_shaders_sky_atmospheric_frag_spv_start, (size_t)(&_binary_shaders_sky_atmospheric_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);
            break;
        default:
            PipelineSettingSetShader(&setting, &_binary_shaders_sky_atmospheric_vert_spv_start, (size_t)(&_binary_shaders_sky_atmospheric_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
            PipelineSettingSetShader(&setting, &_binary_shaders_sky_atmospheric_frag_spv_start, (size_t)(&_binary_shaders_sky_atmospheric_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);
            break;
    }

    setting.fromFile = 0;
    setting.vert_indx = 0;

    GameObject2DAddSettingPipeline(so, nums, &setting);

    so->go.graphObj.blueprints.num_blue_print_packs ++;
}
