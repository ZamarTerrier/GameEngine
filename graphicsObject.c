
#include "graphicsObject.h"

#include "bindDesciption.h"
#include "pipeline.h"
#include "buffers.h"

#include "e_resource.h"

#include "gameObject2D.h"
#include "gameObject3D.h"
#include "models.h"

void GraphicsObject2DInit(GraphicsObject* graphObj){

    graphObj->aShader.bindingDescription = GameObject2DGetBindingDescription();

    graphObj->aShader.attr = planeAttributeDescription;
    graphObj->aShader.countAttr = 3;

    graphObj->shape.rebuild = true;
}

void GraphicsObject3DInit(GraphicsObject* graphObj){

    graphObj->aShader.bindingDescription = GameObject3DGetBindingDescription();

    graphObj->aShader.attr = cubeAttributeDescription;
    graphObj->aShader.countAttr = 4;

    graphObj->shape.rebuild = true;
}

void GraphicsObjectModel3DInit(GraphicsObject* graphObj){

    graphObj->aShader.bindingDescription = ModelObject3DGetBindingDescription();

    graphObj->aShader.attr = modelAttributeDescription;
    graphObj->aShader.countAttr = 6;

    graphObj->shape.rebuild = true;
}

void GraphicsObjectSetVertex(GraphicsObject* graphObj, void *vert, int vertCount, uint32_t *inx, int indxCount){

    graphObj->shape.vParam.vertices = vert;
    graphObj->shape.vParam.verticesSize = vertCount;
    graphObj->shape.iParam.indices = inx;
    graphObj->shape.iParam.indexesSize = indxCount;
    graphObj->shape.rebuild = true;
}

void GraphicsObjectCreateDrawItems(GraphicsObject* graphObj){

    createDescriptorSetLayout(&graphObj->gItems, graphObj->local.descriptors, graphObj->local.descrCount);
    createDescriptorPool(&graphObj->gItems, graphObj->local.descriptors, graphObj->local.descrCount);
    createDescriptorSets(&graphObj->gItems, &graphObj->local);

}

void GraphicsObjectCleanPipelines(GraphicsObject *graphObj){

    for(int i=0;i < graphObj->gItems.pipelineCount; i++){
        vkDestroyPipeline(device, graphObj->gItems.graphicsPipeline[i], NULL);
        vkDestroyPipelineLayout(device, graphObj->gItems.pipelineLayout[i], NULL);
    }
    \

    free(graphObj->gItems.graphicsPipeline);
    graphObj->gItems.graphicsPipeline = NULL;
    free(graphObj->gItems.pipelineLayout);
    graphObj->gItems.pipelineLayout = NULL;
    graphObj->gItems.pipelineCount = 0;
}

void GraphicsObjectClean(GraphicsObject *graphObj)
{
    GraphicsObjectCleanPipelines(graphObj);

    vkDestroyDescriptorPool(device, graphObj->gItems.descriptorPool, NULL);
    vkDestroyDescriptorSetLayout(device, graphObj->gItems.descriptorSetLayout, NULL);


    for(int i=0;i< graphObj->local.descrCount;i++)
    {
        if(graphObj->local.descriptors[i].descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){
            destroyTexture(graphObj->local.descriptors[i].texture);
        }
        else{
            for (int j = 0; j < imagesCount; j++) {
                vkDestroyBuffer(device, graphObj->local.descriptors[i].uniform->uniformBuffers[j], NULL);
                vkFreeMemory(device, graphObj->local.descriptors[i].uniform->uniformBuffersMemory[j], NULL);
            }
            free(graphObj->local.descriptors[i].uniform);
            graphObj->local.descriptors[i].uniform = NULL;
        }
    }
}

void GraphicsObjectDestroy(GraphicsObject* graphObj){

    for(int i=0;i < graphObj->gItems.pipelineCount; i++){
        vkDestroyPipeline(device, graphObj->gItems.graphicsPipeline[i], NULL);
        vkDestroyPipelineLayout(device, graphObj->gItems.pipelineLayout[i], NULL);
    }

    vkDestroyDescriptorPool(device, graphObj->gItems.descriptorPool, NULL);
    vkDestroyDescriptorSetLayout(device, graphObj->gItems.descriptorSetLayout, NULL);


    vkDestroyBuffer(device, graphObj->shape.iParam.indexBuffer, NULL);
    vkFreeMemory(device, graphObj->shape.iParam.indexBufferMemory, NULL);
    //free(graphObj->shape.iParam.indices);

    vkDestroyBuffer(device, graphObj->shape.vParam.vertexBuffer, NULL);
    vkFreeMemory(device, graphObj->shape.vParam.vertexBufferMemory, NULL);
    //free(graphObj->shape.vParam.vertices);

    for(int i=0;i< graphObj->local.descrCount;i++)
    {
        ShaderBuffer *descriptor = &graphObj->local.descriptors[i];
        if(descriptor->descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){
            destroyTexture(descriptor->texture);
            free(descriptor->texture);
            descriptor->texture = NULL;

//            if(descriptor->image != NULL)
//            {
//                if(descriptor->image->path != NULL)
//                    free(descriptor->image->path);

//                if(descriptor->image->pixels != NULL)
//                    free(descriptor->image->pixels);

//                free(descriptor->image);
//            }
        }
        else{
            for (int j = 0; j < imagesCount; j++) {
                vkDestroyBuffer(device, descriptor->uniform->uniformBuffers[j], NULL);
                vkFreeMemory(device, descriptor->uniform->uniformBuffersMemory[j], NULL);
            }
            free(descriptor->uniform);
            descriptor->uniform = NULL;
        }
    }

    free(graphObj->local.descriptors);
    graphObj->local.descriptors = NULL;
}

void GraphicsObjectSetShadersPath(GraphicsObject *graphObj, const char *vert, const char *frag)
{

    int len = strlen(vert);
    memset(graphObj->aShader.vertShader, 0, 256);
    memcpy(graphObj->aShader.vertShader, vert, len);


    len = strlen(frag);
    memset(graphObj->aShader.fragShader, 0, 256);
    memcpy(graphObj->aShader.fragShader, frag, len);
}
