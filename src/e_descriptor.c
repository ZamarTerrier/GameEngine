#include "e_descriptor.h"

#include <vulkan/vulkan.h>

#include "gameObject2D.h"
#include "render_texture.h"

#include "e_descriptor.h"

#include "e_texture_variables.h"

#include "e_resource_engine.h"

void DescriptorUpdate(BluePrintDescriptor *descriptor, char *data, uint32_t size_data)
{
    void *point;
    vkMapMemory(e_device, descriptor->uniform.uniformBuffersMemory[imageIndex], 0, size_data, 0, &point);
    memcpy(point, data, size_data);
    vkUnmapMemory(e_device, descriptor->uniform.uniformBuffersMemory[imageIndex]);
}

void DescriptorSetImage(VkWriteDescriptorSet* descriptorWrites, void *descr_set, uint32_t bind_indx, uint32_t array_size, Texture2D *texture, BluePrintDescriptor *blueprint_descriptor)
{
    descriptorWrites->pImageInfo = calloc(array_size, sizeof(VkDescriptorImageInfo));

    VkDescriptorImageInfo* imageInfo = descriptorWrites->pImageInfo;

    Texture2D **textures = blueprint_descriptor->textures;

    for(int i=0;i < array_size;i++)
    {
        imageInfo[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        if((blueprint_descriptor->flags & ENGINE_BLUE_PRINT_FLAG_SINGLE_IMAGE) && (blueprint_descriptor->flags & ENGINE_BLUE_PRINT_FLAG_ARRAY_IMAGE)){
            imageInfo[i].imageView = textures[i]->textureImageView;
            imageInfo[i].sampler = textures[i]->textureSampler;
        }
        else if(blueprint_descriptor->flags & ENGINE_BLUE_PRINT_FLAG_SINGLE_IMAGE){
            imageInfo[i].imageView = texture->textureImageView;
            imageInfo[i].sampler = texture->textureSampler;
        }
        else{
            imageInfo[i].imageView = texture[i].textureImageView;
            imageInfo[i].sampler = texture[i].textureSampler;
        }

    }

    descriptorWrites->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites->dstSet = descr_set;
    descriptorWrites->dstBinding = bind_indx;
    descriptorWrites->dstArrayElement = 0;
    descriptorWrites->descriptorCount = blueprint_descriptor->descrCount;
    descriptorWrites->descriptorType = blueprint_descriptor->descrType;
}

void DescriptorSetBuffer(VkWriteDescriptorSet* descriptorWrites, void *descr_set, uint32_t bind_indx, void *uniform_buffer, BluePrintDescriptor *blueprint_descriptor)
{
    descriptorWrites->pBufferInfo = calloc(1, sizeof(VkDescriptorBufferInfo));

    VkDescriptorBufferInfo *bufferInfo = descriptorWrites->pBufferInfo;

    bufferInfo->buffer = uniform_buffer;//юнибавер
    bufferInfo->offset = 0;
    bufferInfo->range = blueprint_descriptor->uniform.size;//размер юниформ бафера

    descriptorWrites->sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites->dstSet = descr_set;
    descriptorWrites->dstBinding = bind_indx;
    descriptorWrites->dstArrayElement = 0;
    descriptorWrites->descriptorCount = blueprint_descriptor->descrCount;
    descriptorWrites->descriptorType = blueprint_descriptor->descrType;
}

void DescriptorCreate(ShaderDescriptor *descriptor, BluePrintDescriptor *descriptors, Blueprints *blueprints, size_t num_descr, size_t num_frame) {

    //Создаем параметры дескриптора
    {
        VkDescriptorSetLayoutBinding* bindings = (VkDescriptorSetLayoutBinding *) calloc(num_descr, sizeof(VkDescriptorSetLayoutBinding));

        for(int i=0;i<num_descr;i++)
        {
            bindings[i].binding = i;
            bindings[i].descriptorType = descriptors[i].descrType;
            bindings[i].descriptorCount = descriptors[i].descrCount;
            bindings[i].pImmutableSamplers = NULL;
            bindings[i].stageFlags = descriptors[i].stageflag;
        }

        VkDescriptorSetLayoutCreateInfo layoutInfo = {};
        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layoutInfo.bindingCount = num_descr;
        layoutInfo.pBindings = bindings;

        if (vkCreateDescriptorSetLayout(e_device, &layoutInfo, NULL, &descriptor->descr_set_layout) != VK_SUCCESS) {
            printf("failed to create descriptor set layout!");
            exit(1);
        }

        free(bindings);
        bindings = NULL;
    }

    //Создаем пулл дескрипторов для шейдера
    {
        VkDescriptorPoolSize* poolSizes = (VkDescriptorPoolSize *) calloc(num_descr, sizeof(VkDescriptorPoolSize));

        for(int i=0;i < num_descr; i++)
        {
            poolSizes[i].type = descriptors[i].descrType;
            //[Разобраться в корректном настраивании этого параметра]
            poolSizes[i].descriptorCount = descriptors[i].descrCount * num_frame;
        }

        VkDescriptorPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = num_descr;
        poolInfo.pPoolSizes = poolSizes;
        poolInfo.maxSets = num_frame;
        poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

        if (vkCreateDescriptorPool(e_device, &poolInfo, NULL, &descriptor->descr_pool) != VK_SUCCESS) {
            printf("failed to create descriptor pool!");
            exit(1);
        }

        free(poolSizes);
        poolSizes = NULL;

        //Создаем сами дескрипторы
        //-------------------------
        //Создаем идентичные друг другу сеты дескрипторов
        VkDescriptorSetLayout* layouts = (VkDescriptorSetLayout*) calloc(num_frame, sizeof(VkDescriptorSetLayout));
        for(int i=0; i < num_frame;i++)
        {
            layouts[i] = descriptor->descr_set_layout;
        }

        VkDescriptorSetAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptor->descr_pool;
        allocInfo.descriptorSetCount = num_frame;
        allocInfo.pSetLayouts = layouts;

        descriptor->descr_sets = (VkDescriptorSet*) calloc(num_frame, sizeof(VkDescriptorSet));
        if (vkAllocateDescriptorSets(e_device, &allocInfo, descriptor->descr_sets) != VK_SUCCESS) {
            printf("failed to allocate descriptor sets!");
            exit(1);
        }

        free(layouts);
        layouts = NULL;
    }

    VkWriteDescriptorSet* descriptorWrites = (VkWriteDescriptorSet *) calloc(num_descr, sizeof(VkWriteDescriptorSet));

    //-------------------------------------------------
    //Дескрипторы для всех изображений
    for (int i = 0; i < num_frame; i++) {
        for(int j=0;j < num_descr;j++)
        {
            BluePrintDescriptor *blueprint_descriptor = &descriptors[j];

            if(blueprint_descriptor->descrType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER){

                //Дескриптор Юнибафферов

                DescriptorSetBuffer(&descriptorWrites[j], descriptor->descr_sets[i], j, blueprint_descriptor->uniform.uniformBuffers[i], blueprint_descriptor);

             }else if(blueprint_descriptor->descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){

                //Дескриптор Изображений для шейдера
                Texture2D **textures = blueprint_descriptor->textures;

                //Если изображение еденичное и является массивом
                if((blueprint_descriptor->flags & ENGINE_BLUE_PRINT_FLAG_SINGLE_IMAGE) && (blueprint_descriptor->flags & ENGINE_BLUE_PRINT_FLAG_ARRAY_IMAGE))
                {

                    DescriptorSetImage(&descriptorWrites[j], descriptor->descr_sets[i], j, blueprint_descriptor->size, textures[0], blueprint_descriptor);

                //Если изображение еденичное
                }else if(blueprint_descriptor->flags & ENGINE_BLUE_PRINT_FLAG_SINGLE_IMAGE){

                    DescriptorSetImage(&descriptorWrites[j], descriptor->descr_sets[i], j, 1, textures[0], blueprint_descriptor);

                //Просто массив изображений
                }else{

                    DescriptorSetImage(&descriptorWrites[j], descriptor->descr_sets[i], j, blueprint_descriptor->size, textures[i], blueprint_descriptor);

                }

            }
            int temp = 0;
            temp ++;
        }

        //--------------------------------------

        vkUpdateDescriptorSets(e_device, num_descr, descriptorWrites, 0, NULL);

        for(int i=0;i<num_descr; i++)
        {
            BluePrintDescriptor *blueprint_descriptor = &descriptors[i];
            if(blueprint_descriptor->descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER)
                free(descriptorWrites[i].pImageInfo);
            else
                free(descriptorWrites[i].pBufferInfo);
        }
    }




    //--------------------------------------

    free(descriptorWrites);
    descriptorWrites = NULL;
}
