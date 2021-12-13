#include "bindDesciption.h"

#include "gameObject.h"

//Создаем параметры дескриптора
void createDescriptorSetLayout(GraphicItems* gi, VkDescriptorType* descriptors, size_t count) {

    VkDescriptorSetLayoutBinding* bindings = (VkDescriptorSetLayoutBinding *) calloc(sizeof(VkDescriptorSetLayoutBinding), count);

    for(i=0;i<count;i++)
    {
        switch (descriptors[i]) {
        case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
            bindings[i].binding = i;
            bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            bindings[i].descriptorCount = 1;
            bindings[i].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
            break;
        case VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER :
            bindings[i].binding = i;
            bindings[i].descriptorCount = 1;
            bindings[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            bindings[i].pImmutableSamplers = NULL;
            bindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
            break;
        default:
            break;
        }
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
void createDescriptorPool(GraphicItems* gi, VkDescriptorType* descriptors, size_t count) {

    VkDescriptorPoolSize* poolSizes = (VkDescriptorPoolSize *) calloc(sizeof(VkDescriptorPoolSize), count);
    for(i=0;i < count; i++)
    {
        poolSizes[i].type = descriptors[i];
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
    for(i=0; i < imagesCount;i++)
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

    VkWriteDescriptorSet* descriptorWrites = (VkWriteDescriptorSet *) calloc(params->uniformCount + params->texturesCount, sizeof(VkWriteDescriptorSet));
    VkDescriptorImageInfo* imageInfos = (VkDescriptorImageInfo* ) calloc(params->texturesCount, sizeof(VkDescriptorImageInfo));
    VkDescriptorBufferInfo* bufferInfos = (VkDescriptorImageInfo* ) calloc(params->uniformCount, sizeof(VkDescriptorBufferInfo));

    //-------------------------------------------------
    //Дескрипторы для всех изображений
    for (i = 0; i < imagesCount; i++) {

        //Дескриптор Юниформы для шейдера


        for(j=0;j < params->uniformCount;j++)
        {
            bufferInfos[j].buffer = params->uniformBuffers[j][i];//Массив юнибавера
            bufferInfos[j].offset = 0;
            bufferInfos[j].range = params->uniformSizes[j];//рамер юниформ бафера

            descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[j].dstSet = gi->descriptorSets[i];
            descriptorWrites[j].dstBinding = j;
            descriptorWrites[j].dstArrayElement = 0;
            descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[j].descriptorCount = 1;
            descriptorWrites[j].pBufferInfo = &bufferInfos[j];
        }


        //Дескриптор Изображений для шейдера
        for(j=0;j < params->texturesCount;j++)
        {
            imageInfos[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfos[j].imageView = params->textures[j].textureImageView;
            imageInfos[j].sampler = params->textures[j].textureSampler;

            descriptorWrites[j + params->uniformCount].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[j + params->uniformCount].dstSet = gi->descriptorSets[i];
            descriptorWrites[j + params->uniformCount].dstBinding = j + params->uniformCount;
            descriptorWrites[j + params->uniformCount].dstArrayElement = 0;
            descriptorWrites[j + params->uniformCount].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[j + params->uniformCount].descriptorCount = 1;
            descriptorWrites[j + params->uniformCount].pImageInfo = &imageInfos[j];
        }
        //--------------------------------------

        vkUpdateDescriptorSets(device, params->uniformCount + params->texturesCount, descriptorWrites, 0, NULL);

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
