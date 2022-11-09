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

uint32_t findMemoryType(uint32_t typeFilter, uint32_t properties);

void BuffersCreate(uint64_t size, uint32_t usage, uint32_t properties, void** buffer, void** bufferMemory);

void BuffersCopy(void* srcBuffer, void* dstBuffer, uint64_t size);

void BuffersAddUniformObject(localParam* param, uint64_t size, uint32_t type, uint32_t flags);
void BuffersRecreateUniform(localParam* param);

#endif // BUFFERS_H
