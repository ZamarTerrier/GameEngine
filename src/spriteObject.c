#include "spriteObject.h"

#include <vulkan/vulkan.h>

#include "engine_includes.h"

#include "pipeline.h"
#include "buffers.h"

#include "e_blue_print.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_export.h"

void SpriteObjectCreateQuad(SpriteObject *so)
{
    Vertex2D *verts = calloc(4, sizeof(Vertex2D));

    float size = 0.5f;

    verts[0].position.x = -size;
    verts[0].position.y = -size;
    verts[0].texCoord.x = 0;
    verts[0].texCoord.y = 0;

    verts[1].position.x = size;
    verts[1].position.y = -size;
    verts[1].texCoord.x = 1.0f;
    verts[1].texCoord.y = 0;

    verts[2].position.x = size;
    verts[2].position.y = size;
    verts[2].texCoord.x = 1.0f;
    verts[2].texCoord.y = 1.0f;

    verts[3].position.x = -size;
    verts[3].position.y = size;
    verts[3].texCoord.x = 0;
    verts[3].texCoord.y = 1.0f;

    for(int i=0;i < 4;i++)
    {
        verts[i].color = (vec3){ 1, 1, 1};
    }

    uint32_t *tIndx = calloc(6, sizeof(uint32_t));

    uint32_t indx[] = {
      0, 1, 2, 2, 3, 0
    };

    memcpy(tIndx, indx, 6 * sizeof(uint32_t));

    GraphicsObjectSetVertex(&so->go.graphObj, 0, verts, 4, tIndx, 6);
}

void SpriteObjectInit(SpriteObject *so, DrawParam *dParam){

    GameObject2DInit(so);

    GraphicsObjectSetVertexSize(&so->go.graphObj, 0, sizeof(Vertex2D), sizeof(uint32_t));

    SpriteObjectCreateQuad(so);

    so->go.image = calloc(1, sizeof(GameObjectImage));

    if(strlen(dParam->diffuse) != 0)
    {
        int len = strlen(dParam->diffuse);
        so->go.image->path = calloc(len + 1, sizeof(char));
        memcpy(so->go.image->path, dParam->diffuse, len);
        so->go.image->path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }

    so->go.graphObj.num_shapes = 1;
}

void SpriteObjectAddDefault(SpriteObject *so, void *render)
{

    uint32_t nums = so->go.graphObj.blueprints.num_blue_print_packs;
    so->go.graphObj.blueprints.blue_print_packs[nums].render_point = render;

    BluePrintAddUniformObject(&so->go.graphObj.blueprints, 0, sizeof(TransformBuffer2D), VK_SHADER_STAGE_VERTEX_BIT);
    BluePrintAddUniformObject(&so->go.graphObj.blueprints, 0, sizeof(ImageBufferObjects), VK_SHADER_STAGE_FRAGMENT_BIT);

    BluePrintAddTextureImage(&so->go.graphObj.blueprints, 0, so->go.image);

    PipelineSetting setting;

    PipelineSettingSetDefault(&so->go.graphObj, &setting);

    setting.vertShader = &_binary_shaders_sprite_vert_spv_start;
    setting.sizeVertShader = (size_t)(&_binary_shaders_sprite_vert_spv_size);
    setting.fragShader = &_binary_shaders_sprite_frag_spv_start;
    setting.sizeFragShader = (size_t)(&_binary_shaders_sprite_frag_spv_size);
    setting.fromFile = 0;
    setting.flags |= ENGINE_PIPELINE_FLAG_FACE_CLOCKWISE;

    GameObject2DAddSettingPipeline(so, nums, &setting);

    so->go.graphObj.blueprints.num_blue_print_packs ++;
}

void SpriteObjectInitDefault(SpriteObject *so, DrawParam *dParam)
{
    SpriteObjectInit(so, dParam);
    SpriteObjectAddDefault(so, dParam->render);
    GameObject2DInitDraw(so);
}
