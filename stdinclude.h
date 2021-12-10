#ifndef STDINC_H
#define STDINC_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef enum { false, true} bool;

typedef struct{

    uint32_t graphicsFamily;
    uint32_t presentFamily;

} QueueFamilyIndices;

typedef struct{
    VkSurfaceCapabilitiesKHR capabilities;
    VkSurfaceFormatKHR* formats;
    uint32_t sizeFormats;
    VkPresentModeKHR* presentModes;
    uint32_t sizeModes;
} SwapChainSupportDetails;

typedef struct{
    float x;
    float y;
} vec2;

typedef struct{
    float x;
    float y;
    float z;
} vec3;

typedef struct{
    float x;
    float y;
    float z;
    float t;
} vec4;

typedef struct{

    float m[4][4];

} mat4;

typedef struct {
    vec2 pos;
    vec3 color;    
    vec2 texCoord;
} Vertex;

typedef struct {
    VkVertexInputAttributeDescription* inputDescription;
    uint32_t size;
} attrDescr;

typedef struct {
    vec2 pos;
    vec2 scale;
} ViewUniformObject;

typedef struct  {
    vec2 pos;
    vec2 scale;
} UniformBufferObject;

typedef struct  {
    vec2 imgOffset;
    vec2 imgScale;
} ImgUniformParam;

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
    Vertex* vertices;
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMemory;
    uint32_t verticesSize;
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
    vertexParam vertex;
    indexParam index;
} Shape;

typedef struct{
    VkBuffer** uniformBuffers;
    VkDeviceMemory** uniformBuffersMemory;
    VkDeviceSize* uniformSizes;
    uint32_t uniformCount;
    uint32_t texturesCount;
    Texture2D* textures;
} localParam;

#endif
