#include "graphicsObject.h"

#include "bindDesciption.h"
#include "pipeline.h"
#include "buffers.h"

#include "resource.h"

#include "gameObject3D.h"

void initGraphicsObject(GraphicsObject* graphObj){

    graphObj->aShader.bindingDescription = getBindingDescription();

    graphObj->aShader.attr = planeAttributeDescription;
    graphObj->aShader.countAttr = 3;

}

void GraphicsObject2DSetVertex(GraphicsObject* graphObj, Vertex2D *vert, int vertCount, uint16_t *inx, int indxCount){

    if(graphObj->shape.vertex.vertices != NULL)
    {
        vkDestroyBuffer(device, graphObj->shape.index.indexBuffer, NULL);
        vkFreeMemory(device, graphObj->shape.index.indexBufferMemory, NULL);

        vkDestroyBuffer(device, graphObj->shape.vertex.vertexBuffer, NULL);
        vkFreeMemory(device, graphObj->shape.vertex.vertexBufferMemory, NULL);
    }

    graphObj->shape.vertex.vertices = vert;
    graphObj->shape.vertex.verticesSize = vertCount;
    graphObj->shape.index.indices = inx;
    graphObj->shape.index.indexesSize = indxCount;

    createVertexBuffer(&graphObj->shape.vertex);
    createIndexBuffer(&graphObj->shape.index);
}

void initGraphicsObject3D(GraphicsObject3D* graphObj){

    graphObj->aShader.bindingDescription = GameObject3DGetBindingDescription();

    graphObj->aShader.attr = cubeAttributeDescription;
    graphObj->aShader.countAttr = 3;

}

void GraphicsObject3DSetVertex(GraphicsObject3D* graphObj, Vertex3D *vert, int vertCount, uint16_t *inx, int indxCount){

    if(graphObj->shape.vertex.vertices != NULL)
    {

        vkDestroyBuffer(device, graphObj->shape.index.indexBuffer, NULL);
        vkFreeMemory(device, graphObj->shape.index.indexBufferMemory, NULL);

        vkDestroyBuffer(device, graphObj->shape.vertex.vertexBuffer, NULL);
        vkFreeMemory(device, graphObj->shape.vertex.vertexBufferMemory, NULL);

    }

    graphObj->shape.vertex.vertices = vert;
    graphObj->shape.vertex.verticesSize = vertCount;
    graphObj->shape.index.indices = inx;
    graphObj->shape.index.indexesSize = indxCount;

    createVertexBuffer3D(&graphObj->shape.vertex);
    createIndexBuffer(&graphObj->shape.index);
}

void cleanPipelines(GraphicsObject *graphObj){

    for(int i=0;i < graphObj->gItems.pipelineCount; i++){
        vkDestroyPipeline(device, graphObj->gItems.graphicsPipeline[i], NULL);
        vkDestroyPipelineLayout(device, graphObj->gItems.pipelineLayout[i], NULL);
    }
    \

    graphObj->gItems.graphicsPipeline = (VkPipeline *)realloc(graphObj->gItems.graphicsPipeline,0);
    graphObj->gItems.pipelineLayout = (VkPipeline *)realloc(graphObj->gItems.pipelineLayout,0);
    graphObj->gItems.pipelineCount = 0;
}

void cleanGraphicsObject(GraphicsObject *graphObj)
{
    cleanPipelines(graphObj);

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

    for(int i=0;i < graphObj->gItems.pipelineCount; i++){
        vkDestroyPipeline(device, graphObj->gItems.graphicsPipeline[i], NULL);
        vkDestroyPipelineLayout(device, graphObj->gItems.pipelineLayout[i], NULL);
    }

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

void SetShadersPath(GraphicsObject *graphObj, const char *vert, const char *frag)
{
    graphObj->aShader.vertShader = vert;
    graphObj->aShader.fragShader = frag;
}
