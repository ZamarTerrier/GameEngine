#ifndef PIPELINE_H
#define PIPELINE_H

#include "stdinclude.h"

#include "graphicsObject.h"

#include "resource.h"
#include "tools.h"
#include "bindDesciption.h"

typedef struct{
    int poligonMode;
    int topology;
    const char* vertShader;
    const char* fragShader;
    char drawType;
} PipelineSetting;

void createGraphicsPipeline(GraphicsObject* graphObj);

void createRenderPass();

#endif // PIPELINE_H
