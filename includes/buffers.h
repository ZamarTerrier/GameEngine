#ifndef BUFFERS_H
#define BUFFERS_H

#include "engine_includes.h"

#include "e_blue_print.h"

#include "tools.h"

#define MAX_VERTEX_COUNT 65536
#define MAX_INDEX_COUNT MAX_VERTEX_COUNT * 3

void BuffersCreateCommandPool();

void BuffersCreateCommand();

int BuffersCreateVertexInst(vertexParam* vert);
int BuffersCreateVertex(vertexParam* vert);
int BuffersUpdateVertex(vertexParam* vert);

int BuffersCreateIndexInst(indexParam* ind);
int BuffersCreateIndex(indexParam* ind);
int BuffersUpdateIndex(indexParam* ind);

void BuffersRecreateUniform(Blueprints* blueprints);
void BuffersCreateUniform(UniformStruct* uniform);

uint32_t findMemoryType(uint32_t typeFilter, uint32_t properties);

void BuffersCreate(uint64_t size, uint32_t usage, uint32_t properties, void** buffer, void** bufferMemory);

void BuffersCopy(void* srcBuffer, void* dstBuffer, uint64_t size);


#endif // BUFFERS_H
