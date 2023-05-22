#include "spriteObject.h"

#include <vulkan/vulkan.h>

#include "engine_includes.h"

#include "pipeline.h"
#include "e_buffer.h"

#include "e_blue_print.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_export.h"

void SpriteObjectCreateQuad(SpriteObject *so)
{
    Vertex2D *verts = calloc(4, sizeof(Vertex2D));

    float size = 1.0f;

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

    GraphicsObjectSetVertex(&so->go.graphObj, verts, 4, sizeof(Vertex2D), tIndx, 6, sizeof(uint32_t));
}

void SpriteObjectInit(SpriteObject *so, DrawParam *dParam){

    GameObject2DInit(so);

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

    BluePrintAddUniformObject(&so->go.graphObj.blueprints, 0, sizeof(TransformBuffer2D), VK_SHADER_STAGE_VERTEX_BIT, (void *)GameObject2DTransformBufferUpdate, 0);
    BluePrintAddUniformObject(&so->go.graphObj.blueprints, 0, sizeof(ImageBufferObjects), VK_SHADER_STAGE_FRAGMENT_BIT, (void *)GameObject2DImageBuffer, 0);

    BluePrintAddTextureImage(&so->go.graphObj.blueprints, 0, so->go.image, VK_SHADER_STAGE_FRAGMENT_BIT);

    PipelineSetting setting;

    PipelineSettingSetDefault(&so->go.graphObj, &setting);

    PipelineSettingSetShader(&setting, &_binary_shaders_sprite_vert_spv_start, (size_t)(&_binary_shaders_sprite_vert_spv_size), VK_SHADER_STAGE_VERTEX_BIT);
    PipelineSettingSetShader(&setting, &_binary_shaders_sprite_frag_spv_start, (size_t)(&_binary_shaders_sprite_frag_spv_size), VK_SHADER_STAGE_FRAGMENT_BIT);

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

void SpriteObjectSetOffsetRect(SpriteObject *so, float x, float y, float width, float height)
{
    Vertex2D *verts = so->go.graphObj.shapes[0].vParam.vertices;

    float temp_x = x / so->go.image->imgWidth;
    float temp_y = y / so->go.image->imgHeight;

    verts[0].texCoord.x = temp_x;
    verts[0].texCoord.y = temp_y;

    verts[1].texCoord.x = temp_x + width / so->go.image->imgWidth;
    verts[1].texCoord.y = temp_y;

    verts[2].texCoord.x = temp_x + width / so->go.image->imgWidth;
    verts[2].texCoord.y = temp_y + height / so->go.image->imgHeight;

    verts[3].texCoord.x = temp_x;
    verts[3].texCoord.y = temp_y + height / so->go.image->imgHeight;

    BuffersUpdateVertex(&so->go.graphObj.shapes[0].vParam);

}
