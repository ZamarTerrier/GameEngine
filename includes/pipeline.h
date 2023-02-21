#ifndef PIPELINE_H
#define PIPELINE_H

#include "engine_includes.h"

#include "e_pipeline_variables.h"

#include "graphicsObject.h"

#include "tools.h"
#include "bindDesciption.h"

#ifdef __cplusplus
extern "C"
{
#endif

void PipelineSettingSetDefault(GraphicsObject* graphObj, void *setting);

void PipelineCreateGraphics(GraphicsObject* graphObj);

void PipelineCreateRenderPass();

#ifdef __cplusplus
}
#endif

#endif // PIPELINE_H
