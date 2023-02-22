#include "buffers.h"

#include <vulkan/vulkan.h>

#include "gameObject2D.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"

void BuffersCreateFramebuffers() {
    swapChainFramebuffers = (VkFramebuffer*) calloc(imagesCount, sizeof(VkFramebuffer));

    for (int i=0;i<imagesCount;i++) {
        VkImageView attachments[] = {
            swapChainImageViews[i],
            depthImageView
        };

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 2;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapChainExtent.width;
        framebufferInfo.height = swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(e_device, &framebufferInfo, NULL, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            printf("failed to create framebuffer!");
            exit(1);
        }
    }

}

void BuffersCreateCommandPool() {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(e_physicalDevice);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(e_device, &poolInfo, NULL, &commandPool) != VK_SUCCESS) {
        printf("failed to create command pool!");
        exit(1);
    }

}

void BuffersCreateCommand(){
    commandBuffers = (VkCommandBuffer *) calloc(imagesCount, sizeof(VkCommandBuffer));


    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) imagesCount;

    if (vkAllocateCommandBuffers(e_device, &allocInfo, commandBuffers) != VK_SUCCESS) {
        printf("failed to allocate command buffers!");
        exit(1);
    }

}

void BuffersCreateVertex(vertexParam* vert) {

    if(vert->verticesSize >= MAX_VERTEX_COUNT)
    {
        printf("Очень много вершин!\n");
        return;
    }

    //Выделение памяти
    VkDeviceSize bufferSize;

    bufferSize = vert->typeSize * MAX_VERTEX_COUNT;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vert->vertexBuffer, &vert->vertexBufferMemory);
}

void BuffersUpdateVertex(vertexParam* vert) {

    if(vert->verticesSize >= MAX_VERTEX_COUNT)
    {
        printf("Очень много вершин!\n");
        return;
    }

    //Изменение памяти
    VkDeviceSize bufferSize = vert->typeSize * vert->verticesSize;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    //-------------
    //Перенос данных в буфер

    void* data;
    vkMapMemory(e_device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vert->vertices, (size_t) bufferSize);
    vkUnmapMemory(e_device, stagingBufferMemory);

    //-------------

    BuffersCopy(stagingBuffer, vert->vertexBuffer, bufferSize);

    vkDestroyBuffer(e_device, stagingBuffer, NULL);
    vkFreeMemory(e_device, stagingBufferMemory, NULL);

}

void BuffersCreateIndex(indexParam* ind) {


    if(ind->typeSize >= MAX_INDEX_COUNT)
    {
        printf("Очень много индексов!\n");
        return;
    }

    VkDeviceSize bufferSize = ind->typeSize * MAX_INDEX_COUNT;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &ind->indexBuffer, &ind->indexBufferMemory);
}

void BuffersUpdateIndex(indexParam* ind)
{
    if(ind->typeSize >= MAX_INDEX_COUNT)
    {
        printf("Очень много индексов!\n");
        return;
    }

    VkDeviceSize bufferSize = ind->typeSize * ind->indexesSize;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    void* data;
    vkMapMemory(e_device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, ind->indices, (size_t) bufferSize);
    vkUnmapMemory(e_device, stagingBufferMemory);

    BuffersCopy(stagingBuffer, ind->indexBuffer, bufferSize);

    vkDestroyBuffer(e_device, stagingBuffer, NULL);
    vkFreeMemory(e_device, stagingBufferMemory, NULL);
}

void BuffersCreateUniform(UniformStruct* uniform, int size) {

    uniform->uniformBuffers = (VkBuffer*) calloc(imagesCount, sizeof(VkBuffer));
    uniform->uniformBuffersMemory = (VkDeviceMemory*) calloc(imagesCount, sizeof(VkDeviceMemory));

    for (int i = 0; i < imagesCount; i++) {
        BuffersCreate(uniform->size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &uniform->uniformBuffers[i], &uniform->uniformBuffersMemory[i]);
    }
}

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {

    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(e_physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }

    printf("failed to find suitable memory type!\n");
    exit(1);

}

void BuffersCreate(uint64_t size, uint32_t usage, uint32_t properties, void** buffer, void** bufferMemory) {
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(e_device, &bufferInfo, NULL, buffer) != VK_SUCCESS) {
        printf("failed to create buffer!");
        exit(1);
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(e_device, *buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(e_device, &allocInfo, NULL, bufferMemory) != VK_SUCCESS) {
        printf("failed to allocate buffer memory!");
        exit(1);
    }

    vkBindBufferMemory(e_device, *buffer, *bufferMemory, 0);
}

void BuffersCopy(void* srcBuffer, void* dstBuffer, uint64_t size) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}

void BuffersAddUniformObject(localParam* param, VkDeviceSize size, VkDescriptorType type, VkShaderStageFlags flags){

    if(param->descrCount + 1 >= MAX_UNIFORMS)
    {
        printf("Слишком много декрипторов!\n");
        return;
    }

    ShaderBuffer *descriptor = &param->descriptors[param->descrCount];

    descriptor->uniform = (UniformStruct *) calloc(1, sizeof(UniformStruct));
    descriptor->uniform->size = size;
    descriptor->descrType = type;
    descriptor->size = 1;
    descriptor->stageflag = flags;
    descriptor->buffsize = size;
    descriptor->image = NULL;

    BuffersCreateUniform(descriptor->uniform, param->descrCount);

    param->descrCount ++;
}

void BuffersRecreateUniform(localParam* param){
    for(int i=0;i < param->descrCount;i++){
        ShaderBuffer *descriptor = &param->descriptors[i];

        if(descriptor->descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){

        }
        else
        {
            int sizer = descriptor->buffsize;
            descriptor->uniform = (UniformStruct *) calloc(1, sizeof(UniformStruct));
            descriptor->uniform->size = sizer;
            BuffersCreateUniform(descriptor->uniform, i);
        }
    }
}
