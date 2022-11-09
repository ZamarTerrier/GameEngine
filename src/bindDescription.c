#include "bindDesciption.h"

#include <vulkan/vulkan.h>

#include "gameObject2D.h"

#include "e_texture_variables.h"

//Создаем параметры дескриптора
void createDescriptorSetLayout(GraphicItems* gi, ShaderBuffer* descriptors, size_t count) {

    VkDescriptorSetLayoutBinding* bindings = (VkDescriptorSetLayoutBinding *) calloc(count, sizeof(VkDescriptorSetLayoutBinding));

    for(int i=0;i<count;i++)
    {
        bindings[i].binding = i;
        bindings[i].descriptorType = descriptors[i].descrType;
        bindings[i].descriptorCount = 1;
        bindings[i].pImmutableSamplers = NULL;
        bindings[i].stageFlags = descriptors[i].stageflag;
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = count;
    layoutInfo.pBindings = bindings;

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, NULL, &gi->descriptorSetLayout) != VK_SUCCESS) {
        printf("failed to create descriptor set layout!");
        exit(1);
    }
}
//Создаем пулл дескрипторов для шейдера
void createDescriptorPool(GraphicItems* gi, ShaderBuffer* descriptors, size_t count) {

    VkDescriptorPoolSize* poolSizes = (VkDescriptorPoolSize *) calloc(count, sizeof(VkDescriptorPoolSize));
    for(int i=0;i < count; i++)
    {
        poolSizes[i].type = descriptors[i].descrType;
        poolSizes[i].descriptorCount = imagesCount;
    }

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = count;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.maxSets = imagesCount;

    if (vkCreateDescriptorPool(device, &poolInfo, NULL, &gi->descriptorPool) != VK_SUCCESS) {
        printf("failed to create descriptor pool!");
        exit(1);
    }
}
//Создаем сами дескрипторы
void createDescriptorSets(GraphicItems* gi, localParam* params) {

    //Создаем идентичные друг другу сеты дескрипторов
    VkDescriptorSetLayout* layouts = (VkDescriptorSetLayout*) calloc(imagesCount, sizeof(VkDescriptorSetLayout));
    for(int i=0; i < imagesCount;i++)
    {
        layouts[i] = gi->descriptorSetLayout;
    }

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = gi->descriptorPool;
    allocInfo.descriptorSetCount = imagesCount;
    allocInfo.pSetLayouts = layouts;

    gi->descriptorSets = (VkDescriptorSet*) calloc(imagesCount, sizeof(VkDescriptorSet));
    if (vkAllocateDescriptorSets(device, &allocInfo, gi->descriptorSets) != VK_SUCCESS) {
        printf("failed to allocate descriptor sets!");
        exit(1);
    }

    VkWriteDescriptorSet* descriptorWrites = (VkWriteDescriptorSet *) calloc(params->descrCount, sizeof(VkWriteDescriptorSet));

    int sUniforms = 0, sImages = 0;

    for(int j=0;j < params->descrCount;j++)
    {

        if(params->descriptors[j].descrType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            sUniforms++;
        else if(params->descriptors[j].descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
            sImages++;
    }

    VkDescriptorImageInfo* imageInfos = (VkDescriptorImageInfo *) calloc(sImages, sizeof(VkDescriptorImageInfo));
    VkDescriptorBufferInfo* bufferInfos = (VkDescriptorBufferInfo *) calloc(sUniforms, sizeof(VkDescriptorBufferInfo));

    //-------------------------------------------------
    //Дескрипторы для всех изображений
    for (int i = 0; i < imagesCount; i++) {

        int unisize = 0, textsize = 0;

        for(int j=0;j < params->descrCount;j++)
        {
            ShaderBuffer *descriptor = &params->descriptors[j];
            if(descriptor->descrType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER){
                //Дескриптор Юнибафферов
                    unisize ++;

                    bufferInfos[unisize - 1].buffer = descriptor->uniform->uniformBuffers[i];//Массив юнибавера
                    bufferInfos[unisize - 1].offset = 0;
                    bufferInfos[unisize - 1].range = descriptor->uniform->size;//рамер юниформ бафера

                    descriptorWrites[j].pBufferInfo = &bufferInfos[unisize - 1];
             }else if(params->descriptors[j].descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){
                //Дескриптор Изображений для шейдера
                    textsize ++;
                    Texture2D *texture = descriptor->texture;
                    imageInfos[textsize - 1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    imageInfos[textsize - 1].imageView = texture->textureImageView;
                    imageInfos[textsize - 1].sampler = texture->textureSampler;

                    descriptorWrites[j].pImageInfo = &imageInfos[textsize - 1];
            }

            descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[j].dstSet = gi->descriptorSets[i];
            descriptorWrites[j].dstBinding = j;
            descriptorWrites[j].dstArrayElement = 0;
            descriptorWrites[j].descriptorCount = 1;
            descriptorWrites[j].descriptorType = descriptor->descrType;
        }

        //--------------------------------------

        vkUpdateDescriptorSets(device, params->descrCount, descriptorWrites, 0, NULL);

    }
    //--------------------------------------

    free(bufferInfos);
    bufferInfos = NULL;
    free(imageInfos);
    imageInfos = NULL;
    free(descriptorWrites);
    descriptorWrites = NULL;
    free(layouts);
    layouts = NULL;
}
