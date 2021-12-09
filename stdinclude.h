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

#endif
