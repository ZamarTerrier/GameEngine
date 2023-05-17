#ifndef PIPELINE_H
#define PIPELINE_H

#include "engine_includes.h"

#include "e_pipeline_variables.h"

#include "graphicsObject.h"

#include "e_tools.h"
#include "e_descriptor.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    void *GraphicsPipeline;
    void *GraphicsPipelineLayout;
} PipelineStack;

void PipelineDestroyStack(void *pipeline);

void PipelineClearAll();

void PipelineSettingSetDefault(GraphicsObject* graphObj, void *setting);
void PipelineSettingSetShader(PipelineSetting *setting, char *shader, size_t size, uint32_t type);

void PipelineCreateGraphics(GraphicsObject* graphObj);

void PipelineCreateRenderPass();

void PipelineDestroy(ShaderPack *pipeline);

#ifdef __cplusplus
}
#endif

#endif // PIPELINE_H
