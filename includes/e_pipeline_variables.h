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
    ENGINE_PIPELINE_FLAG_SPECIAL = 0x20,
    ENGINE_PIPELINE_FLAG_ALPHA = 0x40,
    ENGINE_PIPELINE_FLAG_FRAGMENT_SHADER = 0x80,
    ENGINE_PIPELINE_FLAG_COMPUTED_SHADER = 0x100,
    ENGINE_PIPELINE_FLAG_VERTEX_SHADER = 0x200,
    ENGINE_PIPELINE_FLAG_TESSELLATION_CONTROL_SHADER = 0x400,
    ENGINE_PIPELINE_FLAG_TESSELLATION_EVALUATION_SHADER = 0x800,
} EnginePipelineFlags;

typedef struct{
    char* some_shader;
    uint32_t size_some_shader;
    uint32_t type_some_shader;
} EngineShaderStage;

typedef struct{
    char poligonMode;
    char topology;
    char cull_mode;
    EngineShaderStage stages[6];
    uint32_t num_stages;
    char fromFile;
    char dynamicState;
    EPViewport viewport;
    EIRect2D scissor;
    uint32_t flags;
    uint32_t vert_indx;
} PipelineSetting;

#endif // E_PIPELINE_VARIABLES_H
