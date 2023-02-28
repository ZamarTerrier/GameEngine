#ifndef GRAPHICSITEMS_H
#define GRAPHICSITEMS_H

#include "engine_includes.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    uint32_t* code;
    size_t size;
} shader;

typedef struct{
    void* descriptorSetLayout;
    void** descriptorSets;
    void** graphicsPipeline;
    void** pipelineLayout;
    uint32_t pipelineCount;
    void* settings;
    uint32_t settingsCount;
    void* descriptorPool;
    bool perspective;
} GraphicItems;

typedef struct{
    void* vertexBuffer;
    void* vertexBufferMemory;
    uint32_t typeSize;
    uint32_t verticesSize;
    uint32_t bufferSize;
    bool extend;
    void* vertices;
} vertexParam;

typedef struct{
    void* indexBuffer;
    void* indexBufferMemory;
    uint32_t typeSize;
    uint32_t indexesSize;
    uint32_t bufferSize;
    bool extend;
    uint32_t* indices;
} indexParam;

typedef struct{
    char vertShader[256];
    char fragShader[256];
    EIVertexInputBindingDescription* bindingDescription;
    EIVertexInputAttributeDescription* attr;
    size_t countAttr;
} aboutShader;

typedef struct{
    indexParam iParam;
    vertexParam vParam;
    bool linked;
    bool init;
} Shape;

typedef struct{
    void** uniformBuffers;
    void** uniformBuffersMemory;
    uint32_t size;
} UniformStruct;

typedef struct{
    void* texture;
    UniformStruct* uniform;
    uint32_t descrType;
    uint32_t stageflag;
    uint32_t size;
    uint32_t buffsize;
    GameObjectImage *image;
} ShaderBuffer;

typedef struct{
    ShaderBuffer* descriptors;
    uint32_t descrCount;
} localParam;

#ifdef __cplusplus
}
#endif

#endif // GRAPHICSITEMS_H
