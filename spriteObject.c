#include "spriteObject.h"

#include "stdinclude.h"

#include "buffers.h"

void SpriteObjectInit(SpriteObject *so, SpriteParam sParam){

    GameObject2DInit(so);

    Vertex2D *verts = (Vertex2D *) calloc(4, sizeof(Vertex2D));
    memcpy(verts, planeVert, 4 * sizeof(Vertex2D));

    if(sParam.color.x != 0 && sParam.color.y != 0  && sParam.color.z != 0 )
    {
        for(int i=0;i < 4; i++){
            verts[i].color = sParam.color;
        }
    }

    GraphicsObjectSetVertex(&so->go.graphObj, verts, 4, planeIndx, 6);

    GraphicsObjectSetShadersPath(&so->go.graphObj, sParam.vertShader, sParam.fragShader);
    BuffersAddUniformObject(&so->go.graphObj.local, sizeof(TransformBuffer2D), VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT);

    so->go.image = calloc(1, sizeof(ImageStruct));

    if(strlen(sParam.texturePath) != 0)
    {
        so->go.image->path = sParam.texturePath;
        so->go.image->buffer = ToolsLoadImageFromFile(&so->go.image->size, sParam.texturePath);

        ImageAddTexture(&so->go.graphObj.local, so->go.image);
    }


    GameObject2DCreateDrawItems(so);


    PipelineSetting setting;

    setting.poligonMode = VK_POLYGON_MODE_FILL;
    setting.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    setting.vertShader = so->go.graphObj.aShader.vertShader;
    setting.fragShader = so->go.graphObj.aShader.fragShader;
    setting.drawType = 0;
    setting.fromFile = 1;
    setting.scissor.offset.x = 0;
    setting.scissor.offset.y = 0;
    setting.scissor.extent = swapChainExtent;
    setting.viewport.x = 0.0f;
    setting.viewport.y = 0.0f;
    setting.viewport.width = (float) swapChainExtent.width;
    setting.viewport.height = (float) swapChainExtent.height;
    setting.viewport.minDepth = 0.0f;
    setting.viewport.maxDepth = 1.0f;
    GameObject2DAddSettingPipeline(so, setting);

    PipelineCreateGraphics(&so->go.graphObj);

    vec2 tempVec;
    tempVec.x = sParam.position.x;
    tempVec.y = sParam.position.y;

    Transform2DSetPosition(so, tempVec);

    tempVec.x = sParam.size.x;
    tempVec.y = sParam.size.y;
    Transform2DSetScale(so, tempVec);
}
