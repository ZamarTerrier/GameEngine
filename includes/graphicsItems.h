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

enum EngineDescriptorType{
    ENGINE_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    ENGINE_DESCRIPTOR_TYPE_IMAGE_SAMPLER,
    ENGINE_DESCRIPTOR_TYPE_IMAGE_ARRAY
};

typedef struct{
    void *texture;
    void *texture_array;
    uint32_t num_textures;
    uint32_t max_textures;
    UniformStruct* uniform;
    uint32_t descrType;
    uint32_t descrCount;
    uint32_t stageflag;
    uint32_t size;
    uint32_t buffsize;
    GameObjectImage *image;
} ShaderDescriptor;

typedef struct{
    ShaderDescriptor* descriptors;
    uint32_t descrCount;
} localParam;

#ifdef __cplusplus
}
#endif

#endif // GRAPHICSITEMS_H
