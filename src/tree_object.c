#include "tree_object.h"

#include <vulkan/vulkan.h>

#include "pipeline.h"

#include "e_blue_print.h"

#include "e_tools.h"

#include "e_resource_shapes.h"
#include "e_resource_export.h"

void TreeObjectInit(TreeObject *to, DrawParam *dParam)
{
    GameObject3DInit(to);

    vertexParam vParam;
    memset(&vParam, 0, sizeof(vertexParam));
    indexParam iParam;
    memset(&iParam, 0, sizeof(indexParam));

    InitTreeVertices(&vParam, &iParam);

    GraphicsObjectSetVertex(&to->go.graphObj, vParam.vertices, vParam.verticesSize, sizeof(Vertex3D), iParam.indices, iParam.indexesSize, sizeof(uint32_t));
}

void TreeObjectSetDefaultDescriptor(TreeObject *to, DrawParam *dParam)
{
    uint32_t nums = to->go.graphObj.blueprints.num_blue_print_packs;
    to->go.graphObj.blueprints.blue_print_packs[nums].render_point = dParam->render;

    BluePrintAddUniformObject(&to->go.graphObj.blueprints, nums, sizeof(ModelBuffer3D), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject3DDescriptorModelUpdate, 0);

    PipelineSetting setting;

    PipelineSettingSetDefault(&to->go.graphObj, &setting);

    PipelineSettingSetShader(&setting, &_binary_shaders_tree_vert_spv_start, (size_t)(&_binary_shaders_tree_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_tree_frag_spv_start, (size_t)(&_binary_shaders_tree_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);

    setting.fromFile = 0;
    setting.vert_indx = 0;
    setting.cull_mode = VK_CULL_MODE_NONE;
    setting.poligonMode = VK_POLYGON_MODE_LINE;
    //setting.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    //setting.flags &= ~(ENGINE_PIPELINE_FLAG_DRAW_INDEXED);

    GameObject3DAddSettingPipeline(to, nums, &setting);

    to->go.graphObj.blueprints.num_blue_print_packs ++;
}

void TreeObjectInitDefault(TreeObject *to, DrawParam *dParam)
{
    TreeObjectInit(to, dParam);

    TreeObjectSetDefaultDescriptor(to, dParam);

    GameObject3DInitDraw(to);
}
