#include "gameObject.h"

#include "buffers.h"

GameObject initGameObject()
{
    GameObject go;

    go.local.uniformCount = 1;
    go.local.texturesCount = 2;
    go.textures = (Texture2D *) calloc(sizeof(Texture2D), go.local.texturesCount);
    go.textures[0] = createTexture("J:/Projects/GameEngine/textures/texture.png");
    go.textures[1] = createTexture("J:/Projects/GameEngine/textures/texture2.png");

    go.aShader.vertShader = "J:/Projects/GameEngine/shaders/vert.spv";
    go.aShader.fragShader = "J:/Projects/GameEngine/shaders/frag.spv";
    go.aShader.bindingDescription = getBindingDescription();

    attrDescr descr = getAttributeDescriptions();

    go.aShader.attr = descr.inputDescription;
    go.aShader.countAttr = descr.size;

    go.shape.vertex.vertices = vertices;
    go.shape.vertex.verticesSize = 4;
    go.shape.index.indices = indices;
    go.shape.index.indexesSize = 6;

    uint32_t unionSize = go.local.texturesCount + go.local.uniformCount;
    VkDescriptorType* types = (VkDescriptorType *) calloc(sizeof(VkDescriptorType), unionSize) ;

    for(i=0;i < go.local.uniformCount;i++)
    {
        types[i] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }

    for(i=0;i < go.local.texturesCount;i++)
    {
        types[i + go.local.uniformCount] = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    }

    VkDeviceSize uniformSizes[] = {sizeof (UniformBufferObject)};

    createUniformBuffers(&go, uniformSizes, 1);
    createDescriptorSetLayout(&go, types, unionSize);
    createDescriptorPool(&go, types, unionSize);
    createDescriptorSets(&go, go.local.uniformBuffers, uniformSizes);
    createGraphicsPipeline(&go);

    createVertexBuffer(&go);
    createIndexBuffer(&go);

    return go;

}

void recreateDrawningParams(GameObject* go){

    vkDestroyPipeline(device, go->gItems.graphicsPipeline, NULL);
    vkDestroyPipelineLayout(device, go->gItems.pipelineLayout, NULL);
    vkDestroyDescriptorPool(device, go->gItems.descriptorPool, NULL);
    vkDestroyDescriptorSetLayout(device, go->gItems.descriptorSetLayout, NULL);

    uint32_t unionSize = go->local.texturesCount + go->local.uniformCount;
    VkDescriptorType* types = (VkDescriptorType *) calloc(sizeof(VkDescriptorType), unionSize) ;

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
    createDescriptorSets(go, go->local.uniformBuffers, sizeof (UniformBufferObject));
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
    VkVertexInputAttributeDescription* arr = (VkVertexInputAttributeDescription*) calloc(sizeof(VkVertexInputAttributeDescription), 3);

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
        destroyTexture(&go->textures[i]);
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

    go->pos.x += 0.00001f;

    UniformBufferObject ubo = {};
    ubo.model = mat4_f(
                1.f,0,0,0,
                0,1.f,0,0,
                0,0,1.f,0,
                0,0,0,1.f
                       );
    ubo.pos = go->pos;
    void* data;
    vkMapMemory(device, go->local.uniformBuffersMemory[0][imageIndex], 0, sizeof(ubo), 0, &data);
        memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(device, go->local.uniformBuffersMemory[0][imageIndex]);
}
