#include "e_buffer.h"

#include <vulkan/vulkan.h>

#include "gameObject2D.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"

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

int BuffersCreateVertex(vertexParam* vert) {
    //Выделение памяти
    VkDeviceSize bufferSize;

    bufferSize = vert->typeSize * vert->verticesSize;

    if(bufferSize == 0)
        return 1;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vert->vertexBuffer, &vert->vertexBufferMemory);

    vert->bufferSize = bufferSize;
    vert->extend = false;

    return 0;
}

int BuffersCreateVertexInst(vertexParam* vert) {

    if(vert->verticesSize >= MAX_VERTEX_COUNT)
    {
        printf("Очень много вершин!\n");
        return;
    }

    //Выделение памяти
    VkDeviceSize bufferSize;

    bufferSize = vert->typeSize * MAX_VERTEX_COUNT;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &vert->vertexBuffer, &vert->vertexBufferMemory);

    vert->bufferSize = bufferSize;
    vert->extend = true;

    return 0;
}

int BuffersUpdateVertex(vertexParam* vert) {

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    VkDeviceSize bufferSize ;

    bufferSize = vert->typeSize * vert->verticesSize;

    if(!vert->extend){
        if(bufferSize != vert->bufferSize)
            return;
    }
    else{
        if(vert->verticesSize >= MAX_VERTEX_COUNT)
        {
            printf("Очень много вершин!\n");
            return;
        }
    }

    if(vert->extend)
        if(bufferSize != vert->bufferSize)
            return;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    //Изменение памяти
    void* data;
    vkMapMemory(e_device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memset(data, 0, vert->bufferSize);
    memcpy(data, vert->vertices, (size_t) bufferSize);
    vkUnmapMemory(e_device, stagingBufferMemory);

    //-------------

    BuffersCopy(stagingBuffer, vert->vertexBuffer, bufferSize);

    vkDestroyBuffer(e_device, stagingBuffer, NULL);
    vkFreeMemory(e_device, stagingBufferMemory, NULL);

    if(vert->extend)
        vert->bufferSize = bufferSize;

    return 0;
}

int BuffersCreateIndex(indexParam* ind) {

    VkDeviceSize bufferSize = ind->typeSize * ind->indexesSize;

    if(bufferSize == 0)
        return 1;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &ind->indexBuffer, &ind->indexBufferMemory);

    ind->bufferSize = bufferSize;
    ind->extend = false;

    return 0;
}

int BuffersCreateIndexInst(indexParam* ind) {

    if(ind->typeSize >= MAX_INDEX_COUNT)
    {
        printf("Очень много индексов!\n");
        return;
    }

    VkDeviceSize bufferSize = ind->typeSize * MAX_INDEX_COUNT;

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &ind->indexBuffer, &ind->indexBufferMemory);

    ind->bufferSize = bufferSize;
    ind->extend = true;

    return 0;
}

int BuffersUpdateIndex(indexParam* ind)
{

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    VkDeviceSize bufferSize;

    bufferSize = ind->typeSize * ind->indexesSize;

    if(!ind->extend){
        if(bufferSize != ind->bufferSize)
            return;
    }
    else{
        if(ind->indexesSize >= MAX_INDEX_COUNT)
        {
            printf("Очень много индексов!\n");
            return;
        }
    }

    BuffersCreate(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &stagingBuffer, &stagingBufferMemory);

    void* data;
    vkMapMemory(e_device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memset(data, 0, ind->bufferSize);
    memcpy(data, ind->indices, (size_t) bufferSize);
    vkUnmapMemory(e_device, stagingBufferMemory);

    BuffersCopy(stagingBuffer, ind->indexBuffer, bufferSize);

    vkDestroyBuffer(e_device, stagingBuffer, NULL);
    vkFreeMemory(e_device, stagingBufferMemory, NULL);

    if(ind->extend)
        ind->bufferSize = bufferSize;

    return 0;
}

void BuffersCreateUniform(UniformStruct* uniform) {

    uniform->uniformBuffers = (VkBuffer*) calloc(imagesCount, sizeof(VkBuffer));
    uniform->uniformBuffersMemory = (VkDeviceMemory*) calloc(imagesCount, sizeof(VkDeviceMemory));

    for (int i = 0; i < imagesCount; i++) {
        BuffersCreate(uniform->size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &uniform->uniformBuffers[i], &uniform->uniformBuffersMemory[i]);
    }
}

void BuffersCreateStorage(UniformStruct* uniform){
    uniform->uniformBuffers = (VkBuffer*) calloc(imagesCount, sizeof(VkBuffer));
    uniform->uniformBuffersMemory = (VkDeviceMemory*) calloc(imagesCount, sizeof(VkDeviceMemory));

    for (int i = 0; i < imagesCount; i++) {
        BuffersCreate(uniform->size, VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &uniform->uniformBuffers[i], &uniform->uniformBuffersMemory[i]);
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

void BuffersRecreateUniform(Blueprints* blueprints){

    for(int i=0; i < blueprints->num_blue_print_packs;i++)
    {
        BluePrintPack *pack = &blueprints->blue_print_packs[i];

        for(int i=0;i < pack->num_descriptors;i++){
            BluePrintDescriptor *descriptor = &pack->descriptors[i];

            if(descriptor->descrType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
            {
                int sizer = descriptor->buffsize;
                descriptor->uniform->size = sizer;
                BuffersCreateUniform(descriptor->uniform);
            }
        }
    }
}
