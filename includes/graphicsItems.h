#ifndef GRAPHICSITEMS_H
#define GRAPHICSITEMS_H

#include "engine_includes.h"

#define MAX_SHAPES 10

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
    ShaderDescriptor descriptor;
    PipelineStruct pipelines[MAX_PIPELINES];
    uint32_t num_pipelines;
} ShaderPack;

typedef struct{
    ShaderPack shader_packs[MAX_BLUE_PRINTS];
    uint32_t num_shader_packs;
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
} aboutShader;

typedef struct{
    indexParam iParam;
    vertexParam vParam;

    EIVertexInputBindingDescription* bindingDescription;
    EIVertexInputAttributeDescription* attr;
    uint32_t countBind;
    uint32_t countAttr;

    uint32_t type;

    bool init;
} Shape;

enum EngineDescriptorType{
    ENGINE_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
    ENGINE_DESCRIPTOR_TYPE_IMAGE_SAMPLER,
    ENGINE_DESCRIPTOR_TYPE_IMAGE_ARRAY
};

#ifdef __cplusplus
}
#endif

#endif // GRAPHICSITEMS_H
