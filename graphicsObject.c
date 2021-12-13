#include "graphicsObject.h"

#include "bindDesciption.h"
#include "pipeline.h"
#include "buffers.h"

#include "resource.h"

void initGraphicsObject(GraphicsObject* graphObj){

    graphObj->aShader.bindingDescription = getBindingDescription();

    graphObj->aShader.attr = attributeDescription;
    graphObj->aShader.countAttr = 3;

    graphObj->shape.vertex.vertices = vertices;
    graphObj->shape.vertex.verticesSize = 4;
    graphObj->shape.index.indices = indices;
    graphObj->shape.index.indexesSize = 6;

    createVertexBuffer(&graphObj->shape.vertex);
    createIndexBuffer(&graphObj->shape.index);

}


void createDrawningParams(GraphicsObject* graphObj){

    createUniformBuffers(&graphObj->local);

    uint32_t unionSize = graphObj->local.texturesCount + graphObj->local.uniformCount;
    VkDescriptorType* types = (VkDescriptorType *) calloc(unionSize,sizeof(VkDescriptorType)) ;

    for(i=0;i < graphObj->local.uniformCount;i++)
    {
        types[i] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    }

    for(i=0;i < graphObj->local.texturesCount;i++)
    {
        types[i + graphObj->local.uniformCount] = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    }


    createDescriptorSetLayout(&graphObj->gItems, types, unionSize);
    createDescriptorPool(&graphObj->gItems, types, unionSize);
    createDescriptorSets(&graphObj->gItems, &graphObj->local);
    createGraphicsPipeline(graphObj);

    free(types);
    types = NULL;
}

void cleanGraphicsObject(GraphicsObject *graphObj)
{

    vkDestroyPipeline(device, graphObj->gItems.graphicsPipeline, NULL);
    vkDestroyPipelineLayout(device, graphObj->gItems.pipelineLayout, NULL);
    vkDestroyDescriptorPool(device, graphObj->gItems.descriptorPool, NULL);
    vkDestroyDescriptorSetLayout(device, graphObj->gItems.descriptorSetLayout, NULL);


    for (i = 0; i < graphObj->local.uniformCount; i++) {
        for (j = 0; j < imagesCount; j++) {
            vkDestroyBuffer(device, graphObj->local.uniformBuffers[i][j], NULL);
            vkFreeMemory(device, graphObj->local.uniformBuffersMemory[i][j], NULL);
        }
        graphObj->local.uniformBuffers[i] = NULL;
        graphObj->local.uniformBuffersMemory[i] = NULL;
    }

}

void destroyGraphicsObject(GraphicsObject* graphObj){

    vkDestroyPipeline(device, graphObj->gItems.graphicsPipeline, NULL);
    vkDestroyPipelineLayout(device, graphObj->gItems.pipelineLayout, NULL);
    vkDestroyDescriptorPool(device, graphObj->gItems.descriptorPool, NULL);
    vkDestroyDescriptorSetLayout(device, graphObj->gItems.descriptorSetLayout, NULL);

    vkDestroyBuffer(device, graphObj->shape.index.indexBuffer, NULL);
    vkFreeMemory(device, graphObj->shape.index.indexBufferMemory, NULL);

    vkDestroyBuffer(device, graphObj->shape.vertex.vertexBuffer, NULL);
    vkFreeMemory(device, graphObj->shape.vertex.vertexBufferMemory, NULL);

    for(i=0;i< graphObj->local.texturesCount;i++)
    {
        destroyTexture(&graphObj->local.textures[i]);
    }

    for (i = 0; i < graphObj->local.uniformCount; i++) {
        for (j = 0; j < imagesCount; j++) {
            vkDestroyBuffer(device, graphObj->local.uniformBuffers[i][j], NULL);
            vkFreeMemory(device, graphObj->local.uniformBuffersMemory[i][j], NULL);
        }
    }

}
