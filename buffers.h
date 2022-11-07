#ifndef BUFFERS_H
#define BUFFERS_H

#include "stdinclude.h"

#include "e_resource.h"
#include "tools.h"

void BuffersCreateFramebuffers();

void BuffersCreateCommandPool();

void BuffersCreateCommand();

void BufferCreateVertex(vertexParam* vert, uint32_t size);

void BuffersCreateIndex(indexParam* arg, uint32_t type_size);

void BuffersCreateUniform(UniformStruct* uniform, int size);

uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

void BuffersCreate(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer* buffer, VkDeviceMemory* bufferMemory);

void BuffersCopy(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

void BuffersAddUniformObject(localParam* param, VkDeviceSize size, VkDescriptorType type, VkShaderStageFlags flags);
void BuffersRecreateUniform(localParam* param);

#endif // BUFFERS_H
