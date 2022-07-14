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
} Texture2D;

typedef struct{
    VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSet* descriptorSets;
    VkPipeline graphicsPipeline;
    VkPipelineLayout pipelineLayout;
    VkDescriptorPool descriptorPool;
    bool perspective;
} GraphicItems;

typedef struct{
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    uint32_t verticesSize;
    Vertex3D* vertices;
} vertexParam3D;

typedef struct{
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    uint32_t verticesSize;
    Vertex2D* vertices;
} vertexParam;

typedef struct{
    uint32_t* indices;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMemory;
    uint32_t indexesSize;
} indexParam;

typedef struct{
    const char* vertShader;
    const char* fragShader;
    VkVertexInputBindingDescription bindingDescription;
    VkVertexInputAttributeDescription* attr;
    size_t countAttr;
} aboutShader;

typedef struct{
    indexParam index;
    vertexParam vertex;
} Shape;

typedef struct{
    indexParam index;
    vertexParam3D vertex;
} Shape3D;

typedef struct{
    VkBuffer** uniformBuffers;
    VkDeviceMemory** uniformBuffersMemory;
    VkDeviceSize* uniformSizes;
    uint32_t uniformCount;
    uint32_t texturesCount;
    Texture2D* textures;
} localParam;

#endif // GRAPHICSITEMS_H
