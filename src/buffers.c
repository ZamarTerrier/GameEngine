#include "buffers.h"

#include <vulkan/vulkan.h>

#include "gameObject2D.h"

void BuffersCreateFramebuffers() {
    free(swapChainFramebuffers);
    swapChainFramebuffers = NULL;
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

        if (vkCreateFramebuffer(device, &framebufferInfo, NULL, &swapChainFramebuffers[i]) != VK_SUCCESS) {
            printf("failed to create framebuffer!");
            exit(1);
        }
    }
}

void BuffersCreateCommandPool() {
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice);

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(device, &poolInfo, NULL, &commandPool) != VK_SUCCESS) {
        printf("failed to create command pool!");
        exit(1);
    }

}

void BuffersCreateCommand(){
    free(commandBuffers);
    commandBuffers = NULL;
    commandBuffers = (VkCommandBuffer *) calloc(imagesCount, sizeof(VkCommandBuffer));


    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) imagesCount;

    if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers) != VK_SUCCESS) {
        printf("failed to allocate command buffers!");
        exit(1);
    }

}

void BufferCreateVertex(vertexParam* vert, uint32_t size) {

    //Выделение памяти

    vert->typeSize = size;

    VkDeviceSize bufferSize = size * vert->verticesSize;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);
    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vert->vertexBuffer, &vert->vertexBufferMemory);

    //-------------
    //Перенос данных в буфер

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vert->vertices, (size_t) bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    //-------------

    BuffersCopy(stagingBuffer, vert->vertexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);

}

void BuffersCreateIndex(indexParam* ind, uint32_t type_size) {

    VkDeviceSize bufferSize = type_size * ind->indexesSize;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);
    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &ind->indexBuffer, &ind->indexBufferMemory);

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, ind->indices, (size_t) bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    BuffersCopy(stagingBuffer, ind->indexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);
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
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

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

    if (vkCreateBuffer(device, &bufferInfo, NULL, buffer) != VK_SUCCESS) {
        printf("failed to create buffer!");
        exit(1);
    }

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(device, *buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device, &allocInfo, NULL, bufferMemory) != VK_SUCCESS) {
        printf("failed to allocate buffer memory!");
        exit(1);
    }

    vkBindBufferMemory(device, *buffer, *bufferMemory, 0);
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

    param->descrCount ++;

    param->descriptors = (ShaderBuffer *) realloc(param->descriptors, param->descrCount * sizeof(ShaderBuffer));

    param->descriptors[param->descrCount - 1].uniform = (UniformStruct *) calloc(1, sizeof(UniformStruct));
    param->descriptors[param->descrCount - 1].uniform->size = size;
    param->descriptors[param->descrCount - 1].descrType = type;
    param->descriptors[param->descrCount - 1].size = 1;
    param->descriptors[param->descrCount - 1].stageflag = flags;
    param->descriptors[param->descrCount - 1].buffsize = size;
    param->descriptors[param->descrCount - 1].image = NULL;

    BuffersCreateUniform(param->descriptors[param->descrCount - 1].uniform, param->descrCount);

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
