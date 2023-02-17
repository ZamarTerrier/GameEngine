
#include "graphicsObject.h"

#include <vulkan/vulkan.h>

#include "bindDesciption.h"
#include "pipeline.h"
#include "buffers.h"
#include "texture.h"

#include "e_resource.h"

void GraphicsObjectInit(GraphicsObject* graphObj, uint32_t type)
{
    //10 возможных дескрипторов
    graphObj->local.descriptors = (ShaderBuffer *) calloc(MAX_UNIFORMS, sizeof(ShaderBuffer));
    graphObj->local.descrCount = 0;

    //10 возможных настроек для рендеринга
    graphObj->gItems.settings = (PipelineSetting *) calloc(MAX_UNIFORMS, sizeof(PipelineSetting));
    graphObj->gItems.settingsCount = 0;

    switch(type)
    {
        case ENGINE_VERTEX_TYPE_2D_OBJECT:
            graphObj->aShader.bindingDescription = &Bind2DDescription;
            graphObj->aShader.attr = planeAttributeDescription;
            graphObj->aShader.countAttr = 3;
            graphObj->shape.rebuild = true;
            break;
        case ENGINE_VERTEX_TYPE_3D_OBJECT:
            graphObj->aShader.bindingDescription = &Bind3DDescription;
            graphObj->aShader.attr = cubeAttributeDescription;
            graphObj->aShader.countAttr = 4;
            graphObj->shape.rebuild = true;
            break;
        case ENGINE_VERTEX_TYPE_MODEL_OBJECT:
            graphObj->aShader.bindingDescription = &BindModel3DDescription;
            graphObj->aShader.attr = modelAttributeDescription;
            graphObj->aShader.countAttr = 6;
            graphObj->shape.rebuild = true;
            break;
        case ENGINE_TYPE_2D_PARTICLE:
            graphObj->aShader.bindingDescription = &BindParticle2DDescription;
            graphObj->aShader.attr = particle2DAttributeDescription;
            graphObj->aShader.countAttr = 3;
            graphObj->shape.rebuild = true;
            break;
        case ENGINE_TYPE_3D_PARTICLE:
            graphObj->aShader.bindingDescription = &BindParticle3DDescription;
            graphObj->aShader.attr = particle3DAttributeDescription;
            graphObj->aShader.countAttr = 3;
            graphObj->shape.rebuild = true;
            break;
    }
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

        ShaderBuffer *descriptor = &graphObj->local.descriptors[i];
        if(graphObj->local.descriptors[i].descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){

        }
        else{
            for (int j = 0; j < imagesCount; j++) {
                ShaderBuffer *desriptor = &graphObj->local.descriptors[i];
                vkDestroyBuffer(e_device, desriptor->uniform->uniformBuffers[j], NULL);
                vkFreeMemory(e_device, desriptor->uniform->uniformBuffersMemory[j], NULL);
            }
            free(descriptor->uniform);
            descriptor->uniform = NULL;
        }
    }
}

void GraphicsObjectDestroy(GraphicsObject* graphObj){

    free(graphObj->gItems.settings);
    free(graphObj->gItems.graphicsPipeline);
    free(graphObj->gItems.pipelineLayout);
    graphObj->gItems.settings = NULL;
    graphObj->gItems.graphicsPipeline = NULL;
    graphObj->gItems.pipelineLayout = NULL;

    vkFreeDescriptorSets(e_device, graphObj->gItems.descriptorPool, imagesCount, graphObj->gItems.descriptorSets);
    vkDestroyDescriptorPool(e_device, graphObj->gItems.descriptorPool, NULL);
    vkDestroyDescriptorSetLayout(e_device, graphObj->gItems.descriptorSetLayout, NULL);
    graphObj->gItems.descriptorPool = NULL;
    graphObj->gItems.descriptorSetLayout = NULL;
    graphObj->gItems.descriptorSets = NULL;

    vkDestroyBuffer(e_device, graphObj->shape.iParam.indexBuffer, NULL);
    vkFreeMemory(e_device, graphObj->shape.iParam.indexBufferMemory, NULL);
    //free(graphObj->shape.iParam.indices);
    graphObj->shape.iParam.indexBuffer = NULL;
    graphObj->shape.iParam.indexBufferMemory = NULL;
    graphObj->shape.iParam.indices = NULL;

    vkDestroyBuffer(e_device, graphObj->shape.vParam.vertexBuffer, NULL);
    vkFreeMemory(e_device, graphObj->shape.vParam.vertexBufferMemory, NULL);
    //free(graphObj->shape.vParam.vertices);
    graphObj->shape.vParam.vertexBuffer = NULL;
    graphObj->shape.vParam.vertexBufferMemory = NULL;
    graphObj->shape.vParam.vertices = NULL;

    for(int i=0;i < graphObj->local.descrCount;i++)
    {
        ShaderBuffer *descriptor = &graphObj->local.descriptors[i];
        if(descriptor->descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){
            descriptor->texture = NULL;
        }else{
            for (int j = 0; j < imagesCount; j++) {
                vkDestroyBuffer(e_device, descriptor->uniform->uniformBuffers[j], NULL);
                vkFreeMemory(e_device, descriptor->uniform->uniformBuffersMemory[j], NULL);
            }
            free(descriptor->uniform);
            descriptor->uniform = NULL;
        }
    }

    free(graphObj->local.descriptors);
    graphObj->local.descriptors = NULL;
    graphObj->local.descrCount = 0;
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
