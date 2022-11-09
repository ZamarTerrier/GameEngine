#ifndef E_PIPELINE_VARIABLES_H
#define E_PIPELINE_VARIABLES_H

#include <stdint.h>

#include "stdinclude.h"

typedef struct EPViewport {
    float    x;
    float    y;
    float    width;
    float    height;
    float    minDepth;
    float    maxDepth;
} EPViewport;

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
    EPViewport viewport;
    EIRect2D scissor;
} PipelineSetting;

#endif // E_PIPELINE_VARIABLES_H
