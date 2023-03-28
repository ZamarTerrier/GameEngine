#include "quad_object.h"

#include <vulkan/vulkan.h>

#include "pipeline.h"
#include "buffers.h"
#include "camera.h"

#include "e_math.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_export.h"
#include "e_resource_shapes.h"

void QuadObjectUpdate(GameObject2D *go)
{
    TransformBuffer2D tbo;

    tbo.position = v2_subs(go->transform.position, 0.5f);
    tbo.rotation = go->transform.rotation;
    tbo.scale = go->transform.scale;

    DescriptorUpdate(&go->graphObj.blueprints, 0, 0, &tbo, sizeof(tbo));
}

void QuadObjectInit(QuadObject *qu)
{
    GameObject2DInit(qu);

    GameObjectSetUpdateFunc(qu, (void *)QuadObjectUpdate);

    GraphicsObjectSetVertexSize(&qu->go.graphObj, sizeof(Vertex2D), sizeof(uint32_t));
    GraphicsObjectSetVertex(&qu->go.graphObj, projPlaneVert, 4, projPlaneIndx, 6);

    GameObject2DSetLinkedShape(qu);

}

void QuadObjectAddDefault(QuadObject *qu, void *render)
{
    uint32_t nums = qu->go.graphObj.blueprints.num_blue_print_packs;
    qu->go.graphObj.blueprints.blue_print_packs[nums].render_point = render;

    BluePrintAddUniformObject(&qu->go.graphObj.blueprints, nums, sizeof(TransformBuffer2D), VK_SHADER_STAGE_VERTEX_BIT);

    PipelineSetting setting = {};

    PipelineSettingSetDefault(&qu->go.graphObj, &setting);

    if(strlen(setting.vertShader) == 0 || strlen(setting.fragShader) == 0)
    {
        setting.vertShader = &_binary_shaders_quad_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_quad_vert_spv_size);
        setting.fragShader = &_binary_shaders_quad_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_quad_frag_spv_size);
        setting.fromFile = 0;
    }

    GameObject2DAddSettingPipeline(qu, nums, &setting);

    qu->go.graphObj.blueprints.num_blue_print_packs ++;
}
