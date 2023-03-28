#ifndef E_PIPELINE_VARIABLES_H
#define E_PIPELINE_VARIABLES_H

#include <stdint.h>

#include "engine_includes.h"

typedef struct EPViewport {
    float    x;
    float    y;
    float    width;
    float    height;
    float    minDepth;
    float    maxDepth;
} EPViewport;

typedef enum{
    ENGINE_PIPELINE_FLAG_DYNAMIC_VIEW = 0x1,
    ENGINE_PIPELINE_FLAG_FACE_CLOCKWISE = 0x2,
    ENGINE_PIPELINE_FLAG_DRAW_INDEXED = 0x4,
    ENGINE_PIPELINE_FLAG_PERSPECTIVE = 0x8,
    ENGINE_PIPELINE_FLAG_BIAS = 0x10,
} EnginePipelineFlags;

typedef struct{
    char poligonMode;
    char topology;
    char cull_mode;
    char* vertShader;
    uint32_t sizeVertShader;
    char* fragShader;
    uint32_t sizeFragShader;
    char fromFile;
    char dynamicState;
    EPViewport viewport;
    EIRect2D scissor;
    uint32_t flags;
} PipelineSetting;

#endif // E_PIPELINE_VARIABLES_H
