#ifndef STDINC_H
#define STDINC_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "e_vertex.h"
#include "e_buffers_variables.h"

#include "e_math_variables.h"

#define MAX_FONTS 32
#define MAX_IMAGES 32
#define MAX_PIPELINES 50
#define MAX_UNIFORMS 10
#define MAX_BLUE_PRINTS 10

#define MAX_GUI_CALLBACKS 25

#define MAX_CAMERA_VIEW_DISTANCE 1000
#define ORITO_SIZE 0.0007

#ifdef __cplusplus
extern "C"
{
#endif

#ifndef __cplusplus

typedef enum { false, true} bool;
#endif

typedef struct ChildStack{
    struct ChildStack* before;
    struct ChildStack* next;
    void *node;
} ChildStack;

typedef struct EIExtent2D {
    uint32_t    width;
    uint32_t    height;
} EIExtent2D;

typedef struct EIExtent3D {
    uint32_t    width;
    uint32_t    height;
    uint32_t    depth;
} EIExtent3D;

typedef struct EIOffset2D {
    int32_t    x;
    int32_t    y;
} EIOffset2D;

typedef struct EIOffset3D {
    int32_t    x;
    int32_t    y;
    int32_t    z;
} EIOffset3D;

typedef struct EIRect2D {
    EIOffset2D    offset;
    EIExtent2D    extent;
} EIRect2D;

typedef struct{
    char *path;
    char *buffer;
    int size;
    int imgWidth;
    int imgHeight;
    uint32_t flags;
} GameObjectImage;


typedef struct FontCache{
    char path[2048];
    void *cdata;
    void *info;
    void *texture;
} FontCache;

typedef struct PipelineCache{
    void *GraphicsPipeline;
    void *GraphicsPipelineLayout;
    void *setting;
} PipelineCache;

typedef struct EIVertexInputBindingDescription {
    uint32_t             binding;
    uint32_t             stride;
    uint32_t             inputRate;
} EIVertexInputBindingDescription;

typedef struct EIVertexInputAttributeDescription {
    uint32_t    location;
    uint32_t    binding;
    uint32_t    format;
    uint32_t    offset;
} EIVertexInputAttributeDescription;

typedef struct {
    EIVertexInputAttributeDescription* inputDescription;
    uint32_t size;
} attrDescr;

typedef struct{
    vec3 position;
    vec3 rotation;
} SomeDirLight;

typedef struct{
    char diffuse[256];
    char specular[256];
    char normal[256];
    char second[256];
    char font[256];
    char vertShader[256];
    char fragShader[256];
    char topology;
    char drawType;
} DrawParam;

typedef struct EISurfaceCapabilitiesKHR {
    uint32_t                         minImageCount;
    uint32_t                         maxImageCount;
    EIExtent2D                       currentExtent;
    EIExtent2D                       minImageExtent;
    EIExtent2D                       maxImageExtent;
    uint32_t                         maxImageArrayLayers;
    uint32_t       supportedTransforms;
    uint32_t    currentTransform;
    uint32_t         supportedCompositeAlpha;
    uint32_t                supportedUsageFlags;
} EISurfaceCapabilitiesKHR;

typedef struct{
    uint32_t FindLetter;
    uint16_t IndexLetter;
} FontIndexes;

#ifdef __cplusplus
}
#endif

#endif
