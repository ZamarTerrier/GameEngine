#ifndef PIPELINE_H
#define PIPELINE_H

#include "stdinclude.h"

#include "graphicsObject.h"

#include "e_resource.h"
#include "tools.h"
#include "bindDesciption.h"

void PipelineSettingSetDefault(GraphicsObject* graphObj, PipelineSetting *setting);

void PipelineCreateGraphics(GraphicsObject* graphObj);

void PipelineCreateRenderPass();

#endif // PIPELINE_H
