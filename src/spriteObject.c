#include "spriteObject.h"

#include <vulkan/vulkan.h>

#include "engine_includes.h"

#include "pipeline.h"
#include "buffers.h"

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

    GraphicsObjectSetVertex(&so->go.graphObj, verts, 4, tIndx, 6);
}

void SpriteObjectInit(SpriteObject *so, SpriteParam sParam){

    GameObject2DInit(so);

    GraphicsObjectSetVertexSize(&so->go.graphObj, sizeof(Vertex2D), sizeof(uint32_t));

    SpriteObjectCreateQuad(so);

    BuffersAddUniformObject(&so->go.graphObj.local, sizeof(TransformBuffer2D), VK_SHADER_STAGE_VERTEX_BIT);
    BuffersAddUniformObject(&so->go.graphObj.local, sizeof(ImageBufferObjects), VK_SHADER_STAGE_FRAGMENT_BIT);

    so->go.image = calloc(1, sizeof(GameObjectImage));

    if(strlen(sParam.texturePath) != 0)
    {
        int len = strlen(sParam.texturePath);
        so->go.image->path = calloc(len + 1, sizeof(char));
        memcpy(so->go.image->path, sParam.texturePath, len);
        so->go.image->path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
    }

    TextureImageAdd(&so->go.graphObj.local, so->go.image, 0, 0);

    GraphicsObjectCreateDrawItems(&so->go.graphObj);

    PipelineSetting setting;

    PipelineSettingSetDefault(&so->go.graphObj, &setting);

    if(strlen(setting.vertShader) == 0 || strlen(setting.fragShader) == 0)
    {
        setting.vertShader = &_binary_shaders_sprite_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_sprite_vert_spv_size);
        setting.fragShader = &_binary_shaders_sprite_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_sprite_frag_spv_size);
        setting.fromFile = 0;
    }

    GameObject2DAddSettingPipeline(so, &setting);

    PipelineCreateGraphics(&so->go.graphObj);
}
