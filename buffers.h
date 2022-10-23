#ifndef BUFFERS_H
#define BUFFERS_H

#include "stdinclude.h"

#include "e_resource.h"
#include "tools.h"

void createFramebuffers();

void createCommandPool();

void createCommandBuffers();

void createVertexBuffer3D(vertexParam3D* vert);
void createVertexBuffer2D(vertexParam* arg);

void createIndexBuffer(indexParam* arg);

void createUniformBuffer(UniformStruct* uniform, int size);

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);

void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

void addUniformObject(localParam* param, VkDeviceSize size, VkDescriptorType type, VkShaderStageFlags flags);
void recreateUniformBuffers(localParam* param);

#endif // BUFFERS_H
