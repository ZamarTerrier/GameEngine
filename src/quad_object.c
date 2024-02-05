#include "quad_object.h"

#include <vulkan/vulkan.h>

#include "pipeline.h"
#include "e_buffer.h"
#include "e_camera.h"

#include "render_texture.h"

#include "e_math.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_export.h"
#include "e_resource_shapes.h"

void QuadObjectUpdate(GameObject2D *go, BluePrintDescriptor *descriptor)
{
    TransformBuffer2D tbo;

    tbo.position = v2_subs(go->transform.position, 0.5f);
    tbo.rotation = go->transform.rotation;
    tbo.scale = go->transform.scale;

    DescriptorUpdate(descriptor, &tbo, sizeof(tbo));
}

void QuadObjectInit(QuadObject *qu, uint32_t type)
{
    GameObject2DInit(qu);

    GraphicsObjectSetVertex(&qu->go.graphObj, projPlaneVert, 4, sizeof(Vertex2D), projPlaneIndx, 6, sizeof(uint32_t));

    qu->type = type;

    qu->go.graphObj.num_shapes = 1;
}

void QuadObjectAddDefault(QuadObject *qu, void *render)
{
    uint32_t nums = qu->go.graphObj.blueprints.num_blue_print_packs;
    qu->go.graphObj.blueprints.blue_print_packs[nums].render_point = render;

    BluePrintAddUniformObject(&qu->go.graphObj.blueprints, nums, sizeof(TransformBuffer2D), VK_SHADER_STAGE_VERTEX_BIT, (void *)QuadObjectUpdate, 0);

    PipelineSetting setting = {};

    PipelineSettingSetDefault(&qu->go.graphObj, &setting);

    switch(qu->type == ENGINE_QUAD_TYPE_DEPTH){
        case ENGINE_QUAD_TYPE_DEPTH:
            PipelineSettingSetShader(&setting, &_binary_shaders_quad_frag_spv_start, (size_t)(&_binary_shaders_quad_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);
            break;
        case ENGINE_QUAD_TYPE_CUBE:
            PipelineSettingSetShader(&setting, &_binary_shaders_quad_frag_2_spv_start, (size_t)(&_binary_shaders_quad_frag_2_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);
            break;
        case ENGINE_QUAD_TYPE_IMAGE:
            PipelineSettingSetShader(&setting, &_binary_shaders_quad_frag_3_spv_start, (size_t)(&_binary_shaders_quad_frag_3_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);
            break;
    }

    PipelineSettingSetShader(&setting, &_binary_shaders_quad_vert_spv_start, (size_t)(&_binary_shaders_quad_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);

    setting.flags &= ~(ENGINE_PIPELINE_FLAG_ALPHA);
    setting.fromFile = 0;
    setting.vert_indx = 0;

    GameObject2DAddSettingPipeline(qu, nums, &setting);

    qu->go.graphObj.blueprints.num_blue_print_packs ++;
}
