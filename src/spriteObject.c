#include "spriteObject.h"

#include <vulkan/vulkan.h>

#include "stdinclude.h"

#include "pipeline.h"
#include "buffers.h"

void SpriteObjectInit(GameObject2D *go, SpriteParam sParam){

    GameObject2DInit(go);

    Vertex2D *verts = (Vertex2D *) calloc(4, sizeof(Vertex2D));
    memcpy(verts, planeVert, 4 * sizeof(Vertex2D));

    GraphicsObjectSetVertex(&go->graphObj, verts, 4, planeIndx, 6);

    GraphicsObjectSetShadersPath(&go->graphObj, sParam.vertShader, sParam.fragShader);
    BuffersAddUniformObject(&go->graphObj.local, sizeof(TransformBuffer2D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);
    BuffersAddUniformObject(&go->graphObj.local, sizeof(ImageBufferObjects), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT);

    go->image = calloc(1, sizeof(GameObjectImage));

    if(strlen(sParam.texturePath) != 0)
    {
        int len = strlen(sParam.texturePath);
        go->image->path = calloc(len + 1, sizeof(char));
        memcpy(go->image->path, sParam.texturePath, len);
        go->image->path[len] = '\0';
        //go->image->buffer = ToolsLoadImageFromFile(&go->image->size, dParam.filePath);
        ImageAddTexture(&go->graphObj.local, go->image);
    }

    GameObject2DCreateDrawItems(go);

    PipelineSetting setting;

    PipelineSettingSetDefault(&go->graphObj, &setting);

    if(strlen(setting.vertShader) == 0 || strlen(setting.fragShader) == 0)
    {
        setting.vertShader = &_binary_shaders_sprite_vert_spv_start;
        setting.sizeVertShader = (size_t)(&_binary_shaders_sprite_vert_spv_size);
        setting.fragShader = &_binary_shaders_sprite_frag_spv_start;
        setting.sizeFragShader = (size_t)(&_binary_shaders_sprite_frag_spv_size);
        setting.fromFile = 0;
    }

    GameObject2DAddSettingPipeline(go, &setting);

    PipelineCreateGraphics(&go->graphObj);
}
