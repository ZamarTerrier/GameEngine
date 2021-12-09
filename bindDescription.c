#include "bindDesciption.h"

#include "gameObject.h"

//Создаем параметры дескриптора
void createDescriptorSetLayout(void* arg, VkDescriptorType* descriptors, size_t count) {

    GameObject* go = (GameObject *)arg;

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

    if (vkCreateDescriptorSetLayout(device, &layoutInfo, NULL, &go->gItems.descriptorSetLayout) != VK_SUCCESS) {
        printf("failed to create descriptor set layout!");
        exit(1);
    }
}
//Создаем пулл дескрипторов для шейдера
void createDescriptorPool(void* arg, VkDescriptorType* descriptors, size_t count) {

    GameObject* go = (GameObject *)arg;

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

    if (vkCreateDescriptorPool(device, &poolInfo, NULL, &go->gItems.descriptorPool) != VK_SUCCESS) {
        printf("failed to create descriptor pool!");
        exit(1);
    }
}
//Создаем сами дескрипторы
void createDescriptorSets(void* arg) {

    GameObject* go = (GameObject *)arg;

    //Создаем идентичные друг другу сеты дескрипторов
    VkDescriptorSetLayout* layouts = (VkDescriptorSetLayout*) calloc(imagesCount, sizeof(VkDescriptorSetLayout));
    for(i=0; i < imagesCount;i++)
    {
        layouts[i] = go->gItems.descriptorSetLayout;
    }

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = go->gItems.descriptorPool;
    allocInfo.descriptorSetCount = imagesCount;
    allocInfo.pSetLayouts = layouts;

    go->gItems.descriptorSets = (VkDescriptorSet*) calloc(imagesCount, sizeof(VkDescriptorSet));
    if (vkAllocateDescriptorSets(device, &allocInfo, go->gItems.descriptorSets) != VK_SUCCESS) {
        printf("failed to allocate descriptor sets!");
        exit(1);
    }

    VkWriteDescriptorSet* descriptorWrites = (VkWriteDescriptorSet *) calloc(go->local.uniformCount + go->local.texturesCount, sizeof(VkWriteDescriptorSet));
    VkDescriptorImageInfo* imageInfos = (VkDescriptorImageInfo* ) calloc(go->local.texturesCount, sizeof(VkDescriptorImageInfo));
    VkDescriptorBufferInfo* bufferInfos = (VkDescriptorImageInfo* ) calloc(go->local.uniformCount, sizeof(VkDescriptorBufferInfo));

    //-------------------------------------------------
    //Дескрипторы для всех изображений
    for (i = 0; i < imagesCount; i++) {

        //Дескриптор Юниформы для шейдера


        for(j=0;j < go->local.uniformCount;j++)
        {
            bufferInfos[j].buffer = go->local.uniformBuffers[j][i];//Массив юнибавера
            bufferInfos[j].offset = 0;
            bufferInfos[j].range = go->local.uniformSizes[j];//рамер юниформ бафера

            descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[j].dstSet = go->gItems.descriptorSets[i];
            descriptorWrites[j].dstBinding = j;
            descriptorWrites[j].dstArrayElement = 0;
            descriptorWrites[j].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[j].descriptorCount = 1;
            descriptorWrites[j].pBufferInfo = &bufferInfos[j];
        }


        //Дескриптор Изображений для шейдера
        for(j=0;j < go->local.texturesCount;j++)
        {
            imageInfos[j].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfos[j].imageView = go->local.textures[j].textureImageView;
            imageInfos[j].sampler = go->local.textures[j].textureSampler;

            descriptorWrites[j + go->local.uniformCount].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[j + go->local.uniformCount].dstSet = go->gItems.descriptorSets[i];
            descriptorWrites[j + go->local.uniformCount].dstBinding = j + go->local.uniformCount;
            descriptorWrites[j + go->local.uniformCount].dstArrayElement = 0;
            descriptorWrites[j + go->local.uniformCount].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[j + go->local.uniformCount].descriptorCount = 1;
            descriptorWrites[j + go->local.uniformCount].pImageInfo = &imageInfos[j];
        }
        //--------------------------------------

        vkUpdateDescriptorSets(device, go->local.uniformCount + go->local.texturesCount, descriptorWrites, 0, NULL);

    }
    //--------------------------------------

    free(bufferInfos);
    free(imageInfos);
    free(descriptorWrites);
    free(layouts);
}
