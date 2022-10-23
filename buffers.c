#include "buffers.h"

#include "gameObject2D.h"

void createFramebuffers() {
    free(swapChainFramebuffers);
    swapChainFramebuffers = NULL;
    swapChainFramebuffers = (VkFramebuffer*) calloc(imagesCount, sizeof(VkFramebuffer));

    for (i=0;i<imagesCount;i++) {
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

void createCommandPool() {
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

void createCommandBuffers(){
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

void createVertexBuffer3D(vertexParam3D* vert) {

    //Выделение памяти

    VkDeviceSize bufferSize = sizeof(Vertex3D) * vert->verticesSize;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vert->vertexBuffer, &vert->vertexBufferMemory);

    //-------------
    //Перенос данных в буфер

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vert->vertices, (size_t) bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    //-------------

    copyBuffer(stagingBuffer, vert->vertexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);

}

void createVertexBuffer2D(vertexParam* vert) {

    //Выделение памяти

    VkDeviceSize bufferSize = sizeof(Vertex2D) * vert->verticesSize;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vert->vertexBuffer, &vert->vertexBufferMemory);

    //-------------
    //Перенос данных в буфер

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vert->vertices, (size_t) bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    //-------------

    copyBuffer(stagingBuffer, vert->vertexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);

}

void createIndexBuffer(indexParam* ind) {

    VkDeviceSize bufferSize = sizeof(uint32_t) * ind->indexesSize;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);
    createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &ind->indexBuffer, &ind->indexBufferMemory);

    void* data;
    vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, ind->indices, (size_t) bufferSize);
    vkUnmapMemory(device, stagingBufferMemory);

    copyBuffer(stagingBuffer, ind->indexBuffer, bufferSize);

    vkDestroyBuffer(device, stagingBuffer, NULL);
    vkFreeMemory(device, stagingBufferMemory, NULL);
}

void createUniformBuffer(UniformStruct* uniform, int size) {

    uniform->uniformBuffers = (VkBuffer*) calloc(imagesCount, sizeof(VkBuffer));
    uniform->uniformBuffersMemory = (VkDeviceMemory*) calloc(imagesCount, sizeof(VkDeviceMemory));

    for (i = 0; i < imagesCount; i++) {
        createBuffer(uniform->size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &uniform->uniformBuffers[i], &uniform->uniformBuffersMemory[i]);
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

void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory) {
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

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion = {};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
}

void addUniformObject(localParam* param, VkDeviceSize size, VkDescriptorType type, VkShaderStageFlags flags){

    param->descrCount ++;

    param->descriptors = (ShaderBuffer *) realloc(param->descriptors, param->descrCount * sizeof(ShaderBuffer));

    param->descriptors[param->descrCount - 1].uniform = (UniformStruct *) calloc(1, sizeof(UniformStruct));
    param->descriptors[param->descrCount - 1].uniform->size = size;
    param->descriptors[param->descrCount - 1].descrType = type;
    param->descriptors[param->descrCount - 1].size = 1;
    param->descriptors[param->descrCount - 1].stageflag = flags;
    param->descriptors[param->descrCount - 1].buffsize = size;

    createUniformBuffer(param->descriptors[param->descrCount - 1].uniform, param->descrCount);

}

void recreateUniformBuffers(localParam* param){
    for(int i=0;i < param->descrCount;i++){
        if(param->descriptors[i].descrType == VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER){
            char *path = param->descriptors[i].path;
            param->descriptors[i].texture = (Texture2D *) calloc(1, sizeof(Texture2D));
            Texture2D tempTexture = createTexture(param->descriptors[i].path);
            memcpy(param->descriptors[i].texture, &tempTexture, sizeof(Texture2D)) ;
        }
        else
        {
            int sizer = param->descriptors[i].buffsize;
            param->descriptors[i].uniform = (UniformStruct *) calloc(1, sizeof(UniformStruct));
            param->descriptors[i].uniform->size = sizer;
            createUniformBuffer(param->descriptors[i].uniform, i);
        }
    }
}
