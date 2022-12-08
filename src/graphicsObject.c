
#include "graphicsObject.h"

#include <vulkan/vulkan.h>

#include "bindDesciption.h"
#include "pipeline.h"
#include "buffers.h"

#include "e_resource.h"

#include "particleSystem2D.h"
#include "particleSystem3D.h"
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

void GraphicsObjectParticle2DInit(GraphicsObject* graphObj){

    graphObj->aShader.bindingDescription = Particle2DGetBindingDescription();

    graphObj->aShader.attr = particle2DAttributeDescription;
    graphObj->aShader.countAttr = 3;

    graphObj->shape.rebuild = true;
}

void GraphicsObjectParticle3DInit(GraphicsObject* graphObj){

    graphObj->aShader.bindingDescription = Particle3DGetBindingDescription();

    graphObj->aShader.attr = particle3DAttributeDescription;
    graphObj->aShader.countAttr = 3;

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
        vkDestroyPipeline(e_device, graphObj->gItems.graphicsPipeline[i], NULL);
        vkDestroyPipelineLayout(e_device, graphObj->gItems.pipelineLayout[i], NULL);
    }

    free(graphObj->gItems.graphicsPipeline);
    graphObj->gItems.graphicsPipeline = NULL;
    free(graphObj->gItems.pipelineLayout);
    graphObj->gItems.pipelineLayout = NULL;
    graphObj->gItems.pipelineCount = 0;
}

void GraphicsObjectClean(GraphicsObject *graphObj)
{
    GraphicsObjectCleanPipelines(graphObj);

    vkDestroyDescriptorPool(e_device, graphObj->gItems.descriptorPool, NULL);
    vkDestroyDescriptorSetLayout(e_device, graphObj->gItems.descriptorSetLayout, NULL);


    for(int i=0;i< graphObj->local.descrCount;i++)
    {

        ShaderBuffer *descriptor = graphObj->local.descriptors[i];
        if(graphObj->local.descriptors[i]->descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){

        }
        else{
            for (int j = 0; j < imagesCount; j++) {
                ShaderBuffer *desriptor = graphObj->local.descriptors[i];
                vkDestroyBuffer(e_device, desriptor->uniform->uniformBuffers[j], NULL);
                vkFreeMemory(e_device, desriptor->uniform->uniformBuffersMemory[j], NULL);
            }
            free(descriptor->uniform);
            descriptor->uniform = NULL;
        }
    }
}

void GraphicsObjectDestroy(GraphicsObject* graphObj){

    PipelineSetting** settings = graphObj->gItems.settings;
    for(int i=0;i < graphObj->gItems.pipelineCount; i++){
        vkDestroyPipeline(e_device, graphObj->gItems.graphicsPipeline[i], NULL);
        vkDestroyPipelineLayout(e_device, graphObj->gItems.pipelineLayout[i], NULL);
        free(settings[i]);
    }

    free(graphObj->gItems.settings);
    free(graphObj->gItems.graphicsPipeline);
    graphObj->gItems.graphicsPipeline = NULL;
    free(graphObj->gItems.pipelineLayout);
    graphObj->gItems.pipelineLayout = NULL;

    vkDestroyDescriptorPool(e_device, graphObj->gItems.descriptorPool, NULL);
    vkDestroyDescriptorSetLayout(e_device, graphObj->gItems.descriptorSetLayout, NULL);


    vkDestroyBuffer(e_device, graphObj->shape.iParam.indexBuffer, NULL);
    vkFreeMemory(e_device, graphObj->shape.iParam.indexBufferMemory, NULL);
    //free(graphObj->shape.iParam.indices);

    vkDestroyBuffer(e_device, graphObj->shape.vParam.vertexBuffer, NULL);
    vkFreeMemory(e_device, graphObj->shape.vParam.vertexBufferMemory, NULL);
    //free(graphObj->shape.vParam.vertices);

    for(int i=0;i< graphObj->local.descrCount;i++)
    {
        ShaderBuffer *descriptor = graphObj->local.descriptors[i];
        if(descriptor->descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){
            ImageDestroyTexture(descriptor->texture);
            free(descriptor->texture);
            descriptor->texture = NULL;
        }
        else{
            for (int j = 0; j < imagesCount; j++) {
                vkDestroyBuffer(e_device, descriptor->uniform->uniformBuffers[j], NULL);
                vkFreeMemory(e_device, descriptor->uniform->uniformBuffersMemory[j], NULL);
            }
            free(descriptor->uniform);
            descriptor->uniform = NULL;
        }
        free(descriptor);
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
