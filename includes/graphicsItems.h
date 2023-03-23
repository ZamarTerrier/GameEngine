#ifndef GRAPHICSITEMS_H
#define GRAPHICSITEMS_H

#include "engine_includes.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    char* code;
    size_t size;
} shader;

typedef struct{
    void *pipeline;
    void *layout;
} PipelineStruct;

typedef struct{
    void *descr_pool;
    void *descr_set_layout;
    void **descr_sets;
} ShaderDescriptor;

typedef struct{
    ShaderDescriptor descriptors;
    ShaderDescriptor shadow_descr;
    PipelineStruct *pipelines;
    PipelineStruct shadow;
    uint32_t pipelineCount;
    void* settings;
    uint32_t settingsCount;
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
    UniformStruct uniform;
    uint32_t descrType;
    uint32_t descrCount;
    uint32_t stageflag;
    uint32_t size;
    uint32_t buffsize;
    GameObjectImage *image;
} BluePrintDescriptor;

typedef struct{
    BluePrintDescriptor* descriptors;
    BluePrintDescriptor shadow_descr;
    uint32_t isShadow;
    uint32_t count;
} Blueprints;

#ifdef __cplusplus
}
#endif

#endif // GRAPHICSITEMS_H
