#include "gameObject2D.h"

#include <vulkan/vulkan.h>

#include "buffers.h"
#include "pipeline.h"
#include "camera.h"
#include <math.h>

#include "e_pipeline_variables.h"

void GameObject2DDefaultUpdate(GameObject2D* go) {

    if(go->graphObj.local.descriptors == NULL)
        return;

    void* data;

    Camera2D* cam = (Camera2D*) cam2D;

    ShaderBuffer* sBuffer = go->graphObj.local.descriptors;

    TransformBuffer2D tbo;

    tbo.position = go->transform.position;
    tbo.rotation = go->transform.rotation;
    tbo.scale = go->transform.scale;

    vkMapMemory(device, sBuffer[0].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(tbo), 0, &data);
    memcpy(data, &tbo, sizeof(tbo));
    vkUnmapMemory(device, sBuffer[0].uniform->uniformBuffersMemory[imageIndex]);

    ImageBufferObjects ibo;
    ibo.origin = go->transform.img.origin;
    ibo.offset = go->transform.img.offset;
    ibo.scale = go->transform.img.scale;

    ibo.rotation.x = 0;
    ibo.rotation.y = 0;

    vkMapMemory(device, sBuffer[1].uniform->uniformBuffersMemory[imageIndex], 0, sizeof(ibo), 0, &data);
    memcpy(data, &ibo, sizeof(ibo));
    vkUnmapMemory(device, sBuffer[1].uniform->uniformBuffersMemory[imageIndex]);
}

void GameObject2DDefaultDraw(GameObject2D* go){

    if(go->graphObj.shape.rebuild)
    {

        vkDestroyBuffer(device, go->graphObj.shape.iParam.indexBuffer, NULL);
        vkFreeMemory(device, go->graphObj.shape.iParam.indexBufferMemory, NULL);

        vkDestroyBuffer(device, go->graphObj.shape.vParam.vertexBuffer, NULL);
        vkFreeMemory(device, go->graphObj.shape.vParam.vertexBufferMemory, NULL);

        if(go->graphObj.shape.vParam.verticesSize > 0){
            BufferCreateVertex(&go->graphObj.shape.vParam, sizeof(Vertex2D));
        }

        if(go->graphObj.shape.iParam.indexesSize > 0){
            BuffersCreateIndex(&go->graphObj.shape.iParam, sizeof(uint32_t));
        }

        go->graphObj.shape.rebuild = false;
    }

    for(int i=0; i < go->graphObj.gItems.pipelineCount; i++){
        vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.graphicsPipeline[i]);

        PipelineSetting *settings = &go->graphObj.gItems.settings[i];

        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &settings->viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &settings->scissor);

        VkBuffer vertexBuffers[] = {go->graphObj.shape.vParam.vertexBuffer};
        VkDeviceSize offsets[] = {0};

        vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);
        vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.pipelineLayout[i], 0, 1, &go->graphObj.gItems.descriptorSets[imageIndex], 0, NULL);

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
    PipelineSetting* setting = arg;

    go->graphObj.gItems.settingsCount++;
    go->graphObj.gItems.settings = realloc(go->graphObj.gItems.settings, go->graphObj.gItems.settingsCount * sizeof(PipelineSetting));

    PipelineSetting* settings = (PipelineSetting *) go->graphObj.gItems.settings;

    memcpy(&settings[go->graphObj.gItems.settingsCount - 1], setting, sizeof(PipelineSetting));

}

void GameObject2DCreateDrawItems(GameObject2D* go){

    createDescriptorSetLayout(&go->graphObj.gItems, go->graphObj.local.descriptors, go->graphObj.local.descrCount);
    createDescriptorPool(&go->graphObj.gItems, go->graphObj.local.descriptors, go->graphObj.local.descrCount);
    createDescriptorSets(&go->graphObj.gItems, &go->graphObj.local);

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

    BuffersRecreateUniform(&go->graphObj.local);
    GameObject2DCreateDrawItems(go);
    PipelineCreateGraphics(&go->graphObj);
    Transform2DReposition(go);
    Transform2DRescale(go);

}

//Описание вертекса
EIVertexInputBindingDescription GameObject2DGetBindingDescription() {
    EIVertexInputBindingDescription bindingDescription = {};

    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex2D);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
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
}

void GameObject2DInit(GameObject2D* go)
{
    GameObjectSetUpdateFunc(go, (void *)GameObject2DDefaultUpdate);
    GameObjectSetDrawFunc(go, (void *)GameObject2DDefaultDraw);
    GameObjectSetCleanFunc(go, (void *)GameObject2DClean);
    GameObjectSetRecreateFunc(go, (void *)GameObject2DRecreate);
    GameObjectSetDestroyFunc(go, (void *)GameObject2DDestroy);

    Transform2DInit(&go->transform);
    GraphicsObject2DInit(&go->graphObj);

}

vec2 GameObject2DGetSize(GameObject2D* go)
{
    /*vec2 size;
    size.x = fabs((go->graphObj.shape.vParam.vertices[1].position.x - go->graphObj.shape.vParam.vertices[3].position.x) * (viewSize.x * diffSize.x)) * go->transform.scale.x;
    size.y = fabs((go->graphObj.shape.vParam.vertices[1].position.y - go->graphObj.shape.vParam.vertices[3].position.y) * (viewSize.y * diffSize.y)) * go->transform.scale.y;
    return size;*/
}
