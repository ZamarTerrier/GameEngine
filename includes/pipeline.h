#ifndef PIPELINE_H
#define PIPELINE_H

#include "engine_includes.h"

#include "e_pipeline_variables.h"

#include "graphicsObject.h"

#include "e_resource.h"
#include "tools.h"
#include "bindDesciption.h"

void PipelineSettingSetDefault(GraphicsObject* graphObj, void *setting);

void PipelineCreateGraphics(GraphicsObject* graphObj);

void PipelineCreateRenderPass();

#endif // PIPELINE_H
