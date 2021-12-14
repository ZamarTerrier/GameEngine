#include "gameObject.h"

#include "buffers.h"
#include "camera.h"
#include <math.h>

void initGameObject(GameObject* go)
{
    initTransform(&go->transform);
    initGraphicsObject(&go->graphObj);

    go->graphObj.local.uniformCount = 0;
    go->graphObj.local.texturesCount = 0;

}

void addTexture(GameObject* go, const char* file){

    int temp = go->graphObj.local.texturesCount;

    go->graphObj.local.texturesCount ++;

    if(temp == 0)
    {
        free(go->graphObj.local.textures);
        go->graphObj.local.textures = NULL;
        go->graphObj.local.textures = (Texture2D *) calloc( go->graphObj.local.texturesCount, sizeof(Texture2D));
    }
    else
        go->graphObj.local.textures = (Texture2D *) realloc(go->graphObj.local.textures, sizeof(Texture2D) * go->graphObj.local.texturesCount);

    go->graphObj.local.textures[temp] = createTexture(file);

}

void changeTexture(GameObject* go, int elem, const char* file){

    destroyTexture(&go->graphObj.local.textures[elem]);

    go->graphObj.local.textures[elem] = createTexture(file);

}

void addUniformObject(localParam* param, VkDeviceSize size){

    int temp = param->uniformCount;
    param->uniformCount ++;

    if(temp == 0)
    {
        free(param->uniformSizes);
        param->uniformSizes = NULL;
        param->uniformSizes = (VkDeviceSize *) calloc(param->uniformCount, sizeof(VkDeviceSize));
    }
    else
        param->uniformSizes = (VkDeviceSize *) realloc(param->uniformSizes, param->uniformCount * sizeof(VkDeviceSize));

    param->uniformSizes[temp] = size;

}

void createDrawItemsGameObject(GameObject* go){

    createUniformBuffers(&go->graphObj.local);

    uint32_t unionSize = go->graphObj.local.texturesCount + go->graphObj.local.uniformCount;
    VkDescriptorType* types = (VkDescriptorType *) calloc(unionSize,sizeof(VkDescriptorType)) ;

    for(i=0;i < go->graphObj.local.uniformCount;i++)
    {
        types[i] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }

    for(i=0;i < go->graphObj.local.texturesCount;i++)
    {
        types[i + go->graphObj.local.uniformCount] = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    }


    createDescriptorSetLayout(&go->graphObj.gItems, types, unionSize);
    createDescriptorPool(&go->graphObj.gItems, types, unionSize);
    createDescriptorSets(&go->graphObj.gItems, &go->graphObj.local);
    createGraphicsPipeline(&go->graphObj);

    free(types);
    types = NULL;
}

void cleanGameObject(GameObject* go){
    cleanGraphicsObject(&go->graphObj);
}

//Описание вертекса
VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription = {};

    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

void destroyGameObject(GameObject* go){

    destroyGraphicsObject(&go->graphObj);

}

void gameObjectDraw(GameObject* go){

    vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.graphicsPipeline);

    VkBuffer vertexBuffers[] = {go->graphObj.shape.vertex.vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffers[imageIndex], go->graphObj.shape.index.indexBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->graphObj.gItems.pipelineLayout, 0, 1, &go->graphObj.gItems.descriptorSets[imageIndex], 0, NULL);

    vkCmdDrawIndexed(commandBuffers[imageIndex], go->graphObj.shape.index.indexesSize, 1, 0, 0, 0);
}

void updateUniformBuffer(GameObject* go) {

    Camera* cam = (Camera*) camObj;

    ViewUniformObject vuo = {};
    vuo.pos = cam->pos;
    vuo.scale = cam->scale;

    void* data;

    vkMapMemory(device, go->graphObj.local.uniformBuffersMemory[0][imageIndex], 0, sizeof(vuo), 0, &data);
    memcpy(data, &vuo, sizeof(vuo));
    vkUnmapMemory(device, go->graphObj.local.uniformBuffersMemory[0][imageIndex]);

    UniformBufferObject ubo = {};
    ubo.pos = go->transform.pos;
    ubo.scale = go->transform.scale;

    vkMapMemory(device, go->graphObj.local.uniformBuffersMemory[1][imageIndex], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(device, go->graphObj.local.uniformBuffersMemory[1][imageIndex]);

    ImgUniformParam iup = {};
    iup.imgOffset = go->transform.img.offset;
    iup.imgScale = go->transform.img.scale;

    vkMapMemory(device, go->graphObj.local.uniformBuffersMemory[2][imageIndex], 0, sizeof(iup), 0, &data);
    memcpy(data, &iup, sizeof(iup));
    vkUnmapMemory(device, go->graphObj.local.uniformBuffersMemory[2][imageIndex]);

}

vec2 getSizeGameObject(GameObject* go)
{
    vec2 size;
    size.x = fabs((go->graphObj.shape.vertex.vertices[1].pos.x - go->graphObj.shape.vertex.vertices[3].pos.x) * (viewSize.x * diffSize.x)) * go->transform.scale.x;
    size.y = fabs((go->graphObj.shape.vertex.vertices[1].pos.y - go->graphObj.shape.vertex.vertices[3].pos.y) * (viewSize.y * diffSize.y)) * go->transform.scale.y;
    return size;
}
