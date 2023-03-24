
#include "graphicsObject.h"

#include <vulkan/vulkan.h>

#include "e_descriptor.h"
#include "pipeline.h"
#include "buffers.h"
#include "texture.h"

#include "e_resource_data.h"
#include "e_resource_descriptors.h"
#include "e_resource_engine.h"

void GraphicsObjectInit(GraphicsObject* graphObj, uint32_t type)
{
    //10 возможных дескрипторов
    graphObj->blueprints.descriptors = (BluePrintDescriptor *) calloc(MAX_UNIFORMS, sizeof(BluePrintDescriptor));
    graphObj->blueprints.count = 0;

    //10 возможных настроек для рендеринга
    graphObj->gItems.settings = (PipelineSetting *) calloc(MAX_UNIFORMS, sizeof(PipelineSetting));
    graphObj->gItems.settingsCount = 0;

    graphObj->blueprints.isShadow = false;

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
            graphObj->aShader.countAttr = 3;
            break;
        case ENGINE_VERTEX_TYPE_MODEL_OBJECT:
            graphObj->aShader.bindingDescription = &BindModel3DDescription;
            graphObj->aShader.attr = modelAttributeDescription;
            graphObj->aShader.countAttr = 5;
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

void GraphicsObjectCreateDrawItems(GraphicsObject* graphObj, bool with_shadow){

    DescriptorCreate(&graphObj->gItems.descriptors, graphObj->blueprints.descriptors, graphObj->blueprints.count, imagesCount);

    if(with_shadow)
    {
        graphObj->blueprints.isShadow = true;

        DescriptorCreate(&graphObj->gItems.shadow_descr, &graphObj->blueprints.shadow_descr, 1, imagesCount);
    }
}

void GraphicsObjectCleanPipelines(GraphicsObject *graphObj){
    free(graphObj->gItems.pipelines);
    graphObj->gItems.pipelines = NULL;
    graphObj->gItems.pipelineCount = 0;
}

void GraphicsObjectClean(GraphicsObject *graphObj)
{
    GraphicsObjectCleanPipelines(graphObj);

    vkDestroyDescriptorPool(e_device, graphObj->gItems.descriptors.descr_pool, NULL);
    vkDestroyDescriptorSetLayout(e_device, graphObj->gItems.descriptors.descr_set_layout, NULL);


    for(int i=0;i< graphObj->blueprints.count;i++)
    {

        BluePrintDescriptor *descriptor = &graphObj->blueprints.descriptors[i];
        if(graphObj->blueprints.descriptors[i].descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){
            //descriptor->texture = NULL;
        }
        else
        {
            for (int j = 0; j < imagesCount; j++) {
                BluePrintDescriptor *desriptor = &graphObj->blueprints.descriptors[i];
                vkDestroyBuffer(e_device, desriptor->uniform.uniformBuffers[j], NULL);
                vkFreeMemory(e_device, desriptor->uniform.uniformBuffersMemory[j], NULL);
            }
            free(descriptor->uniform.uniformBuffers);
            descriptor->uniform.uniformBuffers = NULL;
            free(descriptor->uniform.uniformBuffersMemory);
            descriptor->uniform.uniformBuffersMemory = NULL;
        }
    }
}

void GraphicsObjectDestroy(GraphicsObject* graphObj){

    for(int i=0;i < graphObj->gItems.pipelineCount;i++)
    {
        vkDestroyPipeline(e_device, graphObj->gItems.pipelines[i].pipeline, NULL);
        vkDestroyPipelineLayout(e_device, graphObj->gItems.pipelines[i].layout, NULL);
    }

    free(graphObj->gItems.settings);
    free(graphObj->gItems.pipelines);
    graphObj->gItems.settings = NULL;
    graphObj->gItems.pipelines = NULL;

    vkFreeDescriptorSets(e_device, graphObj->gItems.descriptors.descr_pool, imagesCount, graphObj->gItems.descriptors.descr_sets);
    vkDestroyDescriptorPool(e_device, graphObj->gItems.descriptors.descr_pool, NULL);
    vkDestroyDescriptorSetLayout(e_device, graphObj->gItems.descriptors.descr_set_layout, NULL);
    graphObj->gItems.descriptors.descr_pool = NULL;
    graphObj->gItems.descriptors.descr_set_layout = NULL;
    graphObj->gItems.descriptors.descr_sets = NULL;

    for(int i=0;i < graphObj->blueprints.count;i++)
    {
        BluePrintDescriptor *descriptor = &graphObj->blueprints.descriptors[i];
        if(descriptor->descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){
            Texture2D *texture = descriptor->texture;

            if(texture->generated)
            {
                ImageDestroyTexture(texture);
                free(descriptor->texture);
            }

            descriptor->texture = NULL;
        }else if(descriptor->descrType == 0x20){

            free(descriptor->texture);
            descriptor->texture = NULL;

        }else{
            for (int j = 0; j < imagesCount; j++) {
                vkDestroyBuffer(e_device, descriptor->uniform.uniformBuffers[j], NULL);
                vkFreeMemory(e_device, descriptor->uniform.uniformBuffersMemory[j], NULL);
            }
            free(descriptor->uniform.uniformBuffers);
            descriptor->uniform.uniformBuffers = NULL;
            free(descriptor->uniform.uniformBuffersMemory);
            descriptor->uniform.uniformBuffersMemory = NULL;
        }
    }

    free(graphObj->blueprints.descriptors);
    graphObj->blueprints.descriptors = NULL;
    graphObj->blueprints.count = 0;

    if(graphObj->blueprints.isShadow)
    {
        vkDestroyPipeline(e_device, graphObj->gItems.shadow.pipeline, NULL);
        vkDestroyPipelineLayout(e_device, graphObj->gItems.shadow.layout, NULL);

        vkFreeDescriptorSets(e_device, graphObj->gItems.shadow_descr.descr_pool, imagesCount, graphObj->gItems.shadow_descr.descr_sets);
        vkDestroyDescriptorPool(e_device, graphObj->gItems.shadow_descr.descr_pool, NULL);
        vkDestroyDescriptorSetLayout(e_device, graphObj->gItems.shadow_descr.descr_set_layout, NULL);

        BluePrintDescriptor *descriptor = &graphObj->blueprints.shadow_descr;
        for (int j = 0; j < imagesCount; j++) {
            vkDestroyBuffer(e_device, descriptor->uniform.uniformBuffers[j], NULL);
            vkFreeMemory(e_device, descriptor->uniform.uniformBuffersMemory[j], NULL);
        }
        free(descriptor->uniform.uniformBuffers);
        descriptor->uniform.uniformBuffers = NULL;
        free(descriptor->uniform.uniformBuffersMemory);
        descriptor->uniform.uniformBuffersMemory = NULL;
    }

    vkDestroyBuffer(e_device, graphObj->shape.iParam.indexBuffer, NULL);
    vkFreeMemory(e_device, graphObj->shape.iParam.indexBufferMemory, NULL);
    graphObj->shape.iParam.indexBuffer = NULL;
    graphObj->shape.iParam.indexBufferMemory = NULL;

    vkDestroyBuffer(e_device, graphObj->shape.vParam.vertexBuffer, NULL);
    vkFreeMemory(e_device, graphObj->shape.vParam.vertexBufferMemory, NULL);
    graphObj->shape.vParam.vertexBuffer = NULL;
    graphObj->shape.vParam.vertexBufferMemory = NULL;
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
