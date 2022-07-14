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

    graphObj->shape.vertex.vertices = planeVert;
    graphObj->shape.vertex.verticesSize = 4;
    graphObj->shape.index.indices = planeIndx;
    graphObj->shape.index.indexesSize = 6;

    createVertexBuffer(&graphObj->shape.vertex);
    createIndexBuffer(&graphObj->shape.index);

}

void initGraphicsObject3D(GraphicsObject3D* graphObj){

    graphObj->aShader.bindingDescription = GameObject3DGetBindingDescription();

    graphObj->aShader.attr = cubeAttributeDescription;
    graphObj->aShader.countAttr = 3;

    graphObj->shape.vertex.vertices = cubeVert;
    graphObj->shape.vertex.verticesSize = 8;
    graphObj->shape.index.indices = cubeIndx;
    graphObj->shape.index.indexesSize = 36;

    createVertexBuffer3D(&graphObj->shape.vertex);
    createIndexBuffer(&graphObj->shape.index);

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

void SetShadersPath(GraphicsObject *graphObj, const char *vert, const char *frag)
{
    graphObj->aShader.vertShader = vert;
    graphObj->aShader.fragShader = frag;
}
