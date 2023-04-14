#include "quad_object.h"

#include <vulkan/vulkan.h>

#include "pipeline.h"
#include "buffers.h"
#include "camera.h"

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

    GraphicsObjectSetVertexSize(&qu->go.graphObj, 0, sizeof(Vertex2D), sizeof(uint32_t));
    GraphicsObjectSetVertex(&qu->go.graphObj, 0, projPlaneVert, 4, projPlaneIndx, 6);

    qu->type = type;

    qu->go.graphObj.num_shapes = 1;
}

void QuadObjectAddDefault(QuadObject *qu, void *render)
{
    uint32_t nums = qu->go.graphObj.blueprints.num_blue_print_packs;
    qu->go.graphObj.blueprints.blue_print_packs[nums].render_point = render;

    BluePrintAddUniformObject(&qu->go.graphObj.blueprints, nums, sizeof(TransformBuffer2D), VK_SHADER_STAGE_VERTEX_BIT, (void *)QuadObjectUpdate);

    PipelineSetting setting = {};

    PipelineSettingSetDefault(&qu->go.graphObj, &setting);

    if(qu->type == ENGINE_QUAD_TYPE_DEPTH){
        setting.fragShader = &_binary_shaders_quad_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_quad_frag_spv_size);
    }else if(qu->type == ENGINE_QUAD_TYPE_CUBE){
        setting.fragShader = &_binary_shaders_quad_frag_2_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_quad_frag_2_spv_size);
    }

    setting.vertShader = &_binary_shaders_quad_vert_spv_start;
    setting.sizeVertShader = (size_t)(&_binary_shaders_quad_vert_spv_size);
    setting.flags &= ~(ENGINE_PIPELINE_FLAG_ALPHA);

    setting.fromFile = 0;
    setting.vert_indx = 0;

    GameObject2DAddSettingPipeline(qu, nums, &setting);

    qu->go.graphObj.blueprints.num_blue_print_packs ++;
}
