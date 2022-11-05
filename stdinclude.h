#ifndef STDINC_H
#define STDINC_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vulkan/vulkan.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_LINE_CHAR 256
#define MAX_BONES 64

typedef enum { false, true} bool;

typedef struct{
    char *path;
    char *buffer;
    int size;
    int texWidth;
    int texHeight;
    int texChannels;
    void *pixels;
} ImageStruct;

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
    float w;
} vec4;

typedef struct{
    uint8_t x;
    uint8_t y;
    uint8_t z;
    uint8_t w;
} vec4_u8;

typedef struct{
    float m[2][2];
} mat2;

typedef struct{
    float m[3][3];
} mat3;

typedef struct{
    float m[4][4];
} mat4;

typedef struct{
    float m[16];
} mat4_p;

typedef struct{
    vec3 p[3];
} triangle;

typedef struct {
    vec2 position;
    vec3 color;    
    vec2 texCoord;
} Vertex2D;

typedef struct {
    vec3 position;
    vec3 normal;
    vec3 color;
    vec2 texCoord;
} Vertex3D;

typedef struct {
    vec3 position;
    vec3 normal;
    vec3 color;
    vec2 texCoord;
    vec4 joints;
    vec4 weight;
} ModelVertex3D;

typedef struct {
    VkVertexInputAttributeDescription* inputDescription;
    uint32_t size;
} attrDescr;

typedef struct {
    vec2 position;
    vec2 rotation;
    vec2 scale;
} ViewBuffer2D;

typedef struct {
    vec3 position __attribute__ ((aligned (16)));
    vec3 rotation __attribute__ ((aligned (16)));
    vec3 scale __attribute__ ((aligned (16)));
} ViewBuffer3D;

typedef struct {
    vec2 position;
    vec2 rotation;
    vec2 scale;
} TransformBuffer2D;

typedef struct {
    vec3 position __attribute__ ((aligned (16)));
    vec3 rotation __attribute__ ((aligned (16)));
    vec3 scale __attribute__ ((aligned (16)));
} TransformBuffer3D;

typedef struct {
    vec2 imgOffset;
    vec2 imgScale;
} ImgBuffer;

typedef struct{
    float time;
} SomeData_e;

typedef struct{
    mat4 model;
    mat4 view;
    mat4 proj;
} ModelBuffer3D;

typedef struct {
    vec3 position __attribute__ ((aligned (16)));
    vec3 color __attribute__ ((aligned (16)));
}Light;

typedef struct{
    mat4 mats[MAX_BONES];
    int size __attribute__ ((aligned (16)));
} InvMatrixsBuffer;

typedef struct{
    Light lights[16] __attribute__ ((aligned (16)));
    int size __attribute__ ((aligned (16)));
} LightBuffer3D;

typedef struct{
    mat4 model;
} ModelBuffer2D;

typedef struct{
    char poligonMode;
    char topology;
    char* vertShader;
    uint32_t sizeVertShader;
    char* fragShader;
    uint32_t sizeFragShader;
    char drawType;
    char fromFile;
    char dynamicState;
    VkViewport viewport;
    VkRect2D scissor;
} PipelineSetting;

typedef struct{
    char filePath[256];
    char vertShader[256];
    char fragShader[256];
    char topology;
    char drawType;
} DrawParam;

#endif
