
#include "graphicsObject.h"

#include <vulkan/vulkan.h>

#include "bindDesciption.h"
#include "pipeline.h"
#include "buffers.h"
#include "texture.h"

#include "e_resource_data.h"
#include "e_resource_descriptors.h"
#include "e_resource_engine.h"

void GraphicsObjectInit(GraphicsObject* graphObj, uint32_t type)
{
    //10 возможных дескрипторов
    graphObj->local.descriptors = (ShaderDescriptor *) calloc(MAX_UNIFORMS, sizeof(ShaderDescriptor));
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
            break;
        case ENGINE_VERTEX_TYPE_3D_OBJECT:
            graphObj->aShader.bindingDescription = &Bind3DDescription;
            graphObj->aShader.attr = cubeAttributeDescription;
            graphObj->aShader.countAttr = 4;
            break;
        case ENGINE_VERTEX_TYPE_MODEL_OBJECT:
            graphObj->aShader.bindingDescription = &BindModel3DDescription;
            graphObj->aShader.attr = modelAttributeDescription;
            graphObj->aShader.countAttr = 6;
            break;
        case ENGINE_VERTEX_TYPE_2D_PARTICLE:
            graphObj->aShader.bindingDescription = &BindParticle2DDescription;
            graphObj->aShader.attr = particle2DAttributeDescription;
            graphObj->aShader.countAttr = 3;
            break;
        case ENGINE_VERTEX_TYPE_3D_PARTICLE:
            graphObj->aShader.bindingDescription = &BindParticle3DDescription;
            graphObj->aShader.attr = particle3DAttributeDescription;
            graphObj->aShader.countAttr = 3;
            break;
        case ENGINE_VERTEX_TYPE_TERRAIN:
            graphObj->aShader.bindingDescription = &BindTerrainDescription;
            graphObj->aShader.attr = TerrainAttributeDescription;
            graphObj->aShader.countAttr = 3;
            break;
        case ENGINE_VERTEX_TYPE_SKY:
            graphObj->aShader.bindingDescription = &BindSkyDescription;
            graphObj->aShader.attr = SkyAttributeDescription;
            graphObj->aShader.countAttr = 2;
            break;
    }

    graphObj->shape.init = false;
}

void GraphicsObjectSetVertexSize(GraphicsObject* graphObj, uint32_t type_v_size, uint32_t type_i_size)
{
    graphObj->shape.vParam.typeSize = type_v_size;
    graphObj->shape.iParam.typeSize = type_i_size;
}

void GraphicsObjectSetVertex(GraphicsObject* graphObj, void *vert, int vertCount, uint32_t *inx, int indxCount){

    int res = 0;

    if(vert != NULL)
    {
        graphObj->shape.vParam.vertices = vert;
        graphObj->shape.vParam.verticesSize = vertCount;
    }

    if(inx != NULL)
    {
        graphObj->shape.iParam.indices = inx;
        graphObj->shape.iParam.indexesSize = indxCount;
    }

    if(!graphObj->shape.init)
    {
        res = BuffersCreateVertex(&graphObj->shape.vParam);
        if(res)
            return;

        res = BuffersCreateIndex(&graphObj->shape.iParam);
        if(res)
            return;

        graphObj->shape.init = true;
    }

    if(graphObj->shape.vParam.verticesSize > 0)
        BuffersUpdateVertex(&graphObj->shape.vParam);

    if(graphObj->shape.iParam.indexesSize > 0)
        BuffersUpdateIndex(&graphObj->shape.iParam);
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

        ShaderDescriptor *descriptor = &graphObj->local.descriptors[i];
        if(graphObj->local.descriptors[i].descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){
            //descriptor->texture = NULL;
        }
        else
        {
            for (int j = 0; j < imagesCount; j++) {
                ShaderDescriptor *desriptor = &graphObj->local.descriptors[i];
                vkDestroyBuffer(e_device, desriptor->uniform->uniformBuffers[j], NULL);
                vkFreeMemory(e_device, desriptor->uniform->uniformBuffersMemory[j], NULL);
            }
            free(descriptor->uniform->uniformBuffers);
            descriptor->uniform->uniformBuffers = NULL;
            free(descriptor->uniform->uniformBuffersMemory);
            descriptor->uniform->uniformBuffersMemory = NULL;
            free(descriptor->uniform);
            descriptor->uniform = NULL;
        }
    }
}

void GraphicsObjectDestroy(GraphicsObject* graphObj){

    for(int i=0;i < graphObj->gItems.pipelineCount;i++)
    {
        vkDestroyPipeline(e_device, graphObj->gItems.graphicsPipeline[i], NULL);
        vkDestroyPipelineLayout(e_device, graphObj->gItems.pipelineLayout[i], NULL);
    }
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
    graphObj->shape.iParam.indexBuffer = NULL;
    graphObj->shape.iParam.indexBufferMemory = NULL;

    vkDestroyBuffer(e_device, graphObj->shape.vParam.vertexBuffer, NULL);
    vkFreeMemory(e_device, graphObj->shape.vParam.vertexBufferMemory, NULL);
    graphObj->shape.vParam.vertexBuffer = NULL;
    graphObj->shape.vParam.vertexBufferMemory = NULL;

    for(int i=0;i < graphObj->local.descrCount;i++)
    {
        ShaderDescriptor *descriptor = &graphObj->local.descriptors[i];
        if(descriptor->descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){
            Texture2D *texture = descriptor->texture;

            if(texture->generated)
            {
                ImageDestroyTexture(texture);
                free(descriptor->texture);
            }

            descriptor->texture = NULL;
        }else{
            for (int j = 0; j < imagesCount; j++) {
                vkDestroyBuffer(e_device, descriptor->uniform->uniformBuffers[j], NULL);
                vkFreeMemory(e_device, descriptor->uniform->uniformBuffersMemory[j], NULL);
            }
            free(descriptor->uniform->uniformBuffers);
            descriptor->uniform->uniformBuffers = NULL;
            free(descriptor->uniform->uniformBuffersMemory);
            descriptor->uniform->uniformBuffersMemory = NULL;
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
