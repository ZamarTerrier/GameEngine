#ifndef GRAPHICSITEMS_H
#define GRAPHICSITEMS_H

#include "stdinclude.h"

typedef struct{
    uint32_t* code;
    size_t size;
} shader;

typedef struct{
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
    VkSampler textureSampler;
    void *stbi_info;
} Texture2D;

typedef struct{
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet* descriptorSets;
    VkPipeline* graphicsPipeline;
    VkPipelineLayout* pipelineLayout;
    uint32_t pipelineCount;
    void* settings;
    uint32_t settingsCount;
    VkDescriptorPool descriptorPool;
    bool perspective;
} GraphicItems;

typedef struct{
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    uint32_t verticesSize;
    void* vertices;
} vertexParam;

typedef struct{
    uint32_t trianglesSize;
    triangle *triangles;
} triangeParam;

typedef struct{
    uint32_t* indices;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    uint32_t indexesSize;
} indexParam;

typedef struct{
    char vertShader[256];
    char fragShader[256];
    VkVertexInputBindingDescription bindingDescription;
    VkVertexInputAttributeDescription* attr;
    size_t countAttr;
} aboutShader;

typedef struct{
    indexParam iParam;
    vertexParam vParam;
    triangeParam tParam;
    bool rebuild;
} Shape;

typedef struct{
    VkBuffer* uniformBuffers;
    VkDeviceMemory* uniformBuffersMemory;
    VkDeviceSize size;
} UniformStruct;

typedef struct{
    Texture2D* texture;
    UniformStruct* uniform;
    VkDescriptorType descrType;
    VkShaderStageFlags stageflag;
    uint32_t size;
    VkDeviceSize buffsize;
    char path[256];
} ShaderBuffer;

typedef struct{
    ShaderBuffer* descriptors;
    uint32_t descrCount;
} localParam;

#endif // GRAPHICSITEMS_H
