#include "gameObject.h"

#include "buffers.h"
#include "camera.h"
#include <math.h>

GameObject* initGameObject()
{
    GameObject* go = (GameObject *) calloc(1, sizeof(GameObject));

    go->pos.x = 0;
    go->pos.y = 0;
    go->scale.x = 1;
    go->scale.y = 1;
    go->img.offset.x = 0;
    go->img.offset.y = 0;
    go->img.scale.x = 1;
    go->img.scale.y = 1;

    go->local.uniformCount = 0;
    go->local.texturesCount = 0;

    go->aShader.vertShader = "/home/ilia/Projects/Graphics/shaders/vert.spv";
    go->aShader.fragShader = "/home/ilia/Projects/Graphics/shaders/frag.spv";
    go->aShader.bindingDescription = getBindingDescription();

    attrDescr descr = getAttributeDescriptions();

    go->aShader.attr = descr.inputDescription;
    go->aShader.countAttr = descr.size;

    go->shape.vertex.vertices = vertices;
    go->shape.vertex.verticesSize = 4;
    go->shape.index.indices = indices;
    go->shape.index.indexesSize = 6;

    createVertexBuffer(go);
    createIndexBuffer(go);

    return go;

}

void addTexture(GameObject* go, const char* file){

    int temp = go->local.texturesCount;

    go->local.texturesCount ++;

    if(temp == 0)
        go->local.textures = (Texture2D *) calloc( go->local.texturesCount, sizeof(Texture2D));
    else
        go->local.textures = (Texture2D *) realloc(go->local.textures, sizeof(Texture2D) * go->local.texturesCount);

    go->local.textures[temp] = createTexture(file);

}

void changeTexture(GameObject* go, int elem, const char* file){

    destroyTexture(&go->local.textures[elem]);

    go->local.textures[elem] = createTexture(file);

}

void addUniformObject(GameObject* go, VkDeviceSize size){

    int temp = go->local.uniformCount;
    go->local.uniformCount ++;

    if(temp == 0)
        go->local.uniformSizes = (VkDeviceSize *) calloc(go->local.uniformCount, sizeof(VkDeviceSize));
    else
        go->local.uniformSizes = (VkDeviceSize *) realloc(go->local.uniformSizes, go->local.uniformCount * sizeof(VkDeviceSize));

    go->local.uniformSizes[temp] = size;

}

void recreateDrawningParams(GameObject* go){

    if(go->local.uniformBuffers[0] != NULL)
    {
        vkDestroyPipeline(device, go->gItems.graphicsPipeline, NULL);
        vkDestroyPipelineLayout(device, go->gItems.pipelineLayout, NULL);
        vkDestroyDescriptorPool(device, go->gItems.descriptorPool, NULL);
        vkDestroyDescriptorSetLayout(device, go->gItems.descriptorSetLayout, NULL);

        for (i = 0; i < go->local.uniformCount; i++) {
            for (j = 0; j < imagesCount; j++) {
            vkDestroyBuffer(device, go->local.uniformBuffers[i][j], NULL);
            vkFreeMemory(device, go->local.uniformBuffersMemory[i][j], NULL);
            }
        }

        createUniformBuffers(go);
    }

    uint32_t unionSize = go->local.texturesCount + go->local.uniformCount;
    VkDescriptorType* types = (VkDescriptorType *) calloc(unionSize,sizeof(VkDescriptorType)) ;

    for(i=0;i < go->local.uniformCount;i++)
    {
        types[i] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }

    for(i=0;i < go->local.texturesCount;i++)
    {
        types[i + go->local.uniformCount] = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    }


    createDescriptorSetLayout(go, types, unionSize);
    createDescriptorPool(go, types, unionSize);
    createDescriptorSets(go);
    createGraphicsPipeline(go);
}

void createDrawningParams(GameObject* go){

    createUniformBuffers(go);

    uint32_t unionSize = go->local.texturesCount + go->local.uniformCount;
    VkDescriptorType* types = (VkDescriptorType *) calloc(unionSize,sizeof(VkDescriptorType)) ;

    for(i=0;i < go->local.uniformCount;i++)
    {
        types[i] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }

    for(i=0;i < go->local.texturesCount;i++)
    {
        types[i + go->local.uniformCount] = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    }


    createDescriptorSetLayout(go, types, unionSize);
    createDescriptorPool(go, types, unionSize);
    createDescriptorSets(go);
    createGraphicsPipeline(go);
}

//Описание вертекса
VkVertexInputBindingDescription getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription = {};

    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}
//Атрибуты вертекса
attrDescr getAttributeDescriptions() {
    VkVertexInputAttributeDescription* arr = (VkVertexInputAttributeDescription*) calloc(3, sizeof(VkVertexInputAttributeDescription));

    arr[0].binding = 0;
    arr[0].location = 0;
    arr[0].format = VK_FORMAT_R32G32_SFLOAT;
    arr[0].offset = offsetof(Vertex, pos);

    arr[1].binding = 0;
    arr[1].location = 1;
    arr[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    arr[1].offset = offsetof(Vertex, color);

    arr[2].binding = 0;
    arr[2].location = 2;
    arr[2].format = VK_FORMAT_R32G32_SFLOAT;
    arr[2].offset = offsetof(Vertex, texCoord);

    attrDescr temp = {};
    temp.inputDescription = arr;
    temp.size = 3;
    return temp;
}

void destroyGameObject(GameObject* go){

    vkDestroyPipeline(device, go->gItems.graphicsPipeline, NULL);
    vkDestroyPipelineLayout(device, go->gItems.pipelineLayout, NULL);
    vkDestroyDescriptorPool(device, go->gItems.descriptorPool, NULL);
    vkDestroyDescriptorSetLayout(device, go->gItems.descriptorSetLayout, NULL);


    vkDestroyBuffer(device, go->shape.index.indexBuffer, NULL);
    vkFreeMemory(device, go->shape.index.indexBufferMemory, NULL);

    vkDestroyBuffer(device, go->shape.vertex.vertexBuffer, NULL);
    vkFreeMemory(device, go->shape.vertex.vertexBufferMemory, NULL);

    for(i=0;i< go->local.texturesCount;i++)
    {
        destroyTexture(&go->local.textures[i]);
    }

    for (i = 0; i < go->local.uniformCount; i++) {
        for (j = 0; j < imagesCount; j++) {
        vkDestroyBuffer(device, go->local.uniformBuffers[i][j], NULL);
        vkFreeMemory(device, go->local.uniformBuffersMemory[i][j], NULL);
        }
    }

}

void gameObjectDraw(GameObject* go){
    vkCmdBindPipeline(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->gItems.graphicsPipeline);

    VkBuffer vertexBuffers[] = {go->shape.vertex.vertexBuffer};
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffers[imageIndex], 0, 1, vertexBuffers, offsets);

    vkCmdBindIndexBuffer(commandBuffers[imageIndex], go->shape.index.indexBuffer, 0, VK_INDEX_TYPE_UINT16);

    vkCmdBindDescriptorSets(commandBuffers[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, go->gItems.pipelineLayout, 0, 1, &go->gItems.descriptorSets[imageIndex], 0, NULL);

    vkCmdDrawIndexed(commandBuffers[imageIndex], go->shape.index.indexesSize, 1, 0, 0, 0);
}

void updateUniformBuffer(GameObject* go) {

    Camera* cam = (Camera*) camObj;

    ViewUniformObject vuo = {};
    vuo.pos = cam->pos;
    vuo.scale = cam->scale;

    void* data;

    vkMapMemory(device, go->local.uniformBuffersMemory[0][imageIndex], 0, sizeof(vuo), 0, &data);
        memcpy(data, &vuo, sizeof(vuo));
    vkUnmapMemory(device, go->local.uniformBuffersMemory[0][imageIndex]);

    UniformBufferObject ubo = {};
    ubo.pos = go->pos;
    ubo.scale = go->scale;

    vkMapMemory(device, go->local.uniformBuffersMemory[1][imageIndex], 0, sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(device, go->local.uniformBuffersMemory[1][imageIndex]);

    ImgUniformParam iup = {};
    iup.imgOffset = go->img.offset;
    iup.imgScale = go->img.scale;

    vkMapMemory(device, go->local.uniformBuffersMemory[2][imageIndex], 0, sizeof(iup), 0, &data);
        memcpy(data, &iup, sizeof(iup));
    vkUnmapMemory(device, go->local.uniformBuffersMemory[2][imageIndex]);
}

void setScaleGameObject(GameObject* go, vec2 scale)
{
    go->scale = scale;
}

void setPosGameObject(GameObject* go, vec2 pos)
{
    go->pos = pos;
}
