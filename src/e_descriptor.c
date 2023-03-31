#include "e_descriptor.h"

#include <vulkan/vulkan.h>

#include "gameObject2D.h"
#include "render_texture.h"

#include "e_descriptor.h"

#include "e_texture_variables.h"

#include "e_resource_engine.h"

void DescriptorUpdate(Blueprints *blueprints, uint32_t indx_pack, uint32_t indx_descr, char *data, uint32_t size_data)
{
    BluePrintDescriptor *descriptor = &blueprints->blue_print_packs[indx_pack].descriptors[indx_descr];

    void *point;
    vkMapMemory(e_device, descriptor->uniform.uniformBuffersMemory[imageIndex], 0, size_data, 0, &point);
    memcpy(point, data, size_data);
    vkUnmapMemory(e_device, descriptor->uniform.uniformBuffersMemory[imageIndex]);
}

void DescriptorCreate(ShaderDescriptor *descriptor, BluePrintDescriptor *descriptors, Blueprints *blueprints, size_t num_descr, size_t num_frame) {

    //Создаем параметры дескриптора
    VkDescriptorSetLayoutBinding* bindings = (VkDescriptorSetLayoutBinding *) calloc(num_descr, sizeof(VkDescriptorSetLayoutBinding));

    for(int i=0;i<num_descr;i++)
    {
        bindings[i].binding = i;
        bindings[i].descriptorType = descriptors[i].descrType == 0x20 ? VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER : descriptors[i].descrType;
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

    //Создаем пулл дескрипторов для шейдера
    VkDescriptorPoolSize* poolSizes = (VkDescriptorPoolSize *) calloc(num_descr, sizeof(VkDescriptorPoolSize));
    for(int i=0;i < num_descr; i++)
    {
        poolSizes[i].type = descriptors[i].descrType == 0x20 ? VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER : descriptors[i].descrType;
        poolSizes[i].descriptorCount = num_frame;
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

    VkWriteDescriptorSet* descriptorWrites = (VkWriteDescriptorSet *) calloc(num_descr, sizeof(VkWriteDescriptorSet));

    int sUniforms = 0, sImages = 0;

    for(int j=0;j < num_descr;j++)
    {

        if(descriptors[j].descrType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            sUniforms ++;
        else if(descriptors[j].descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER || descriptors[j].descrType == 0x20)
            sImages ++;
    }

    VkDescriptorImageInfo* imageInfos = (VkDescriptorImageInfo *) calloc(sImages, sizeof(VkDescriptorImageInfo));
    VkDescriptorBufferInfo* bufferInfos = (VkDescriptorBufferInfo *) calloc(sUniforms, sizeof(VkDescriptorBufferInfo));

    //-------------------------------------------------
    //Дескрипторы для всех изображений
    for (int i = 0; i < num_frame; i++) {

        int unisize = 0, textsize = 0;

        for(int j=0;j < num_descr;j++)
        {
            BluePrintDescriptor *blueprint_descriptor = &descriptors[j];
            if(blueprint_descriptor->descrType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER){
                //Дескриптор Юнибафферов

                    bufferInfos[unisize].buffer = blueprint_descriptor->uniform.uniformBuffers[i];//Массив юнибавера
                    bufferInfos[unisize].offset = 0;
                    bufferInfos[unisize].range = blueprint_descriptor->uniform.size;//рамер юниформ бафера

                    descriptorWrites[j].pBufferInfo = &bufferInfos[unisize];

                    unisize ++;

             }else if(blueprint_descriptor->descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){
                //Дескриптор Изображений для шейдера

                    Texture2D *texture = blueprint_descriptor->texture;
                    imageInfos[textsize].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    imageInfos[textsize].imageView = texture->textureImageView;
                    imageInfos[textsize].sampler = texture->textureSampler;

                    descriptorWrites[j].pImageInfo = &imageInfos[textsize];

                    textsize ++;
            }else if(blueprint_descriptor->descrType == 0x20){

                RenderTexture *render = blueprint_descriptor->render_image;
                imageInfos[textsize].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                if(render->flags & ENGINE_RENDER_FLAG_ONE_SHOT)
                {
                    imageInfos[textsize].imageView = render->frames[0].view;
                    imageInfos[textsize].sampler = render->frames[0].sampler;
                }else{
                    imageInfos[textsize].imageView = render->frames[i].view;
                    imageInfos[textsize].sampler = render->frames[i].sampler;
                }

                descriptorWrites[j].pImageInfo = &imageInfos[textsize];

                textsize ++;
            }

            descriptorWrites[j].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[j].dstSet = descriptor->descr_sets[i];
            descriptorWrites[j].dstBinding = j;
            descriptorWrites[j].dstArrayElement = 0;
            descriptorWrites[j].descriptorCount = blueprint_descriptor->descrCount;
            descriptorWrites[j].descriptorType = blueprint_descriptor->descrType == 0x20 ? VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER : blueprint_descriptor->descrType;
        }

        //--------------------------------------

        vkUpdateDescriptorSets(e_device, num_descr, descriptorWrites, 0, NULL);

    }
    //--------------------------------------

    free(descriptorWrites);
    descriptorWrites = NULL;
    free(bufferInfos);
    bufferInfos = NULL;
    free(imageInfos);
    imageInfos = NULL;
    free(descriptorWrites);
    descriptorWrites = NULL;
    free(layouts);
    layouts = NULL;
}
