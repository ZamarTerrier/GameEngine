#ifndef BUFFERS_H
#define BUFFERS_H

#include "stdinclude.h"

#include "resource.h"
#include "tools.h"

void createFramebuffers();

void createCommandPool();

void createCommandBuffers();

void createVertexBuffer(void* arg);

void createIndexBuffer(void* arg);

void createUniformBuffers(void* arg);

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

#endif // BUFFERS_H
