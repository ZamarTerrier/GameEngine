#include "gameObject2D.h"

#include <vulkan/vulkan.h>
#include <math.h>

#include "buffers.h"
#include "pipeline.h"
#include "camera.h"

#include "e_math.h"

#include "e_pipeline_variables.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"
#include "e_resource_export.h"

void GameObject2DDefaultUpdate(GameObject2D* go) {

    if(go->graphObj.blueprints.descriptors == NULL)
        return;

    void* data;

    Camera2D* cam = (Camera2D*) cam2D;

    BluePrintDescriptor* sBuffer = go->graphObj.blueprints.descriptors;

    TransformBuffer2D tbo;

    tbo.position = v2_subs(go->transform.position, 0.5f);
    tbo.rotation = go->transform.rotation;
    tbo.scale = go->transform.scale;

    DescriptorUpdate(go->graphObj.blueprints.descriptors, 0, &tbo, sizeof(tbo));

    ImageBufferObjects ibo;
    ibo.origin = go->transform.img.origin;
    ibo.offset = go->transform.img.offset;
    ibo.scale = go->transform.img.scale;

    ibo.rotation.x = 0;
    ibo.rotation.y = 0;

    DescriptorUpdate(go->graphObj.blueprints.descriptors, 1, &ibo, sizeof(ibo));
}

void GameObject2DDefaultDraw(GameObject2D* go){

    /*if(go->graphObj.shape.rebuild)
    {

        vkDestroyBuffer(e_device, go->graphObj.shape.vParam.vertexBuffer, NULL);
        vkFreeMemory(e_device, go->graphObj.shape.vParam.vertexBufferMemory, NULL);
        vkDestroyBuffer(e_device, go->graphObj.shape.iParam.indexBuffer, NULL);
        vkFreeMemory(e_device, go->graphObj.shape.iParam.indexBufferMemory, NULL);

        go->graphObj.shape.vParam.vertexBuffer = NULL;
        go->graphObj.shape.vParam.vertexBufferMemory = NULL;
        go->graphObj.shape.iParam.indexBuffer = NULL;
        go->graphObj.shape.iParam.indexBufferMemory = NULL;

        if(go->graphObj.shape.vParam.verticesSize > 0){
            BufferCreateVertex(&go->graphObj.shape.vParam, sizeof(Vertex2D));
        }

        if(go->graphObj.shape.iParam.indexesSize > 0){
            BuffersCreateIndex(&go->graphObj.shape.iParam, sizeof(uint32_t));
        }

        go->graphObj.shape.rebuild = false;
    }*/

    for(int i=0; i < go->graphObj.gItems.pipelineCount; i++){
        vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.pipelines[i].pipeline);

        PipelineSetting *settings = &go->graphObj.gItems.settings[i];

        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &settings->viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &settings->scissor);

        VkBuffer vertexBuffers[] = {go->graphObj.shape.vParam.vertexBuffer};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);
        vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.pipelines[i].layout, 0, 1, &go->graphObj.gItems.descriptors.descr_sets[imageIndex], 0, NULL);

        switch(settings->drawType){
            case 0:
                vkCmdBindIndexBuffer(commandBuffers[imageIndex], go->graphObj.shape.iParam.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
                vkCmdDrawIndexed(commandBuffers[imageIndex], go->graphObj.shape.iParam.indexesSize, 1, 0, 0, 0);
                break;
            case 1:
                vkCmdDraw(commandBuffers[imageIndex], go->graphObj.shape.vParam.verticesSize, 1, 0, 0);
                break;
        }
    }
}

void GameObject2DAddSettingPipeline(GameObject2D* go, void *arg){

    if(go->graphObj.gItems.settingsCount + 1 > MAX_UNIFORMS)
        return;

    PipelineSetting* setting = arg;


    PipelineSetting* settings = go->graphObj.gItems.settings;
    memcpy(&settings[go->graphObj.gItems.settingsCount], setting, sizeof(PipelineSetting));

    go->graphObj.gItems.settingsCount++;

}

void GameObject2DClean(GameObject2D* go){
    GraphicsObjectClean(&go->graphObj);
}

void GameObject2DRecreate(GameObject2D* go){

    PipelineSetting *settings = (PipelineSetting *)go->graphObj.gItems.settings;

    for(int i=0; i < go->graphObj.gItems.settingsCount;i++)
    {
        settings[i].scissor.offset.x = 0;
        settings[i].scissor.offset.y = 0;
        settings[i].scissor.extent.height = HEIGHT;
        settings[i].scissor.extent.width = WIDTH;
        settings[i].viewport.x = 0;
        settings[i].viewport.y = 0;
        settings[i].viewport.height = HEIGHT;
        settings[i].viewport.width = WIDTH;
    }

    BuffersRecreateUniform(&go->graphObj.blueprints);
    GraphicsObjectCreateDrawItems(&go->graphObj, false);
    PipelineCreateGraphics(&go->graphObj, false);
    Transform2DReposition(go);
    Transform2DRescale(go);

}

void GameObject2DDestroy(GameObject2D* go){

    GraphicsObjectDestroy(&go->graphObj);

    if(go->image != NULL)
    {
        free(go->image->path);

        if(go->image->size > 0)
            free(go->image->buffer);

        free(go->image);
    }

    if(!go->graphObj.shape.linked)
    {
        if(go->graphObj.shape.vParam.verticesSize)
            free(go->graphObj.shape.vParam.vertices);

        if(go->graphObj.shape.iParam.indexesSize)
            free(go->graphObj.shape.iParam.indices);
    }
}

void GameObject2DSetLinkedShape(GameObject2D *go)
{
    go->graphObj.shape.linked = true;
}

void GameObject2DInit(GameObject2D* go)
{
    GameObjectSetUpdateFunc(go, (void *)GameObject2DDefaultUpdate);
    GameObjectSetDrawFunc(go, (void *)GameObject2DDefaultDraw);
    GameObjectSetCleanFunc(go, (void *)GameObject2DClean);
    GameObjectSetRecreateFunc(go, (void *)GameObject2DRecreate);
    GameObjectSetDestroyFunc(go, (void *)GameObject2DDestroy);

    go->self.obj_type = ENGINE_GAME_OBJECT_TYPE_2D;

    Transform2DInit(&go->transform);
    GraphicsObjectInit(&go->graphObj, ENGINE_VERTEX_TYPE_2D_OBJECT);

    go->graphObj.shape.linked = false;
}

vec2 GameObject2DGetSize(GameObject2D* go)
{
    /*vec2 size;
    size.x = fabs((go->graphObj.shape.vParam.vertices[1].position.x - go->graphObj.shape.vParam.vertices[3].position.x) * (viewSize.x * diffSize.x)) * go->transform.scale.x;
    size.y = fabs((go->graphObj.shape.vParam.vertices[1].position.y - go->graphObj.shape.vParam.vertices[3].position.y) * (viewSize.y * diffSize.y)) * go->transform.scale.y;
    return size;*/
}
