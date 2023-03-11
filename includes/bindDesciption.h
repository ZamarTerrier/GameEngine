#ifndef BINDDESCIPTION_H
#define BINDDESCIPTION_H

#include "engine_includes.h"
#include "graphicsItems.h"

void createDescriptorSetLayout(GraphicItems* gi, ShaderDescriptor* descriptors, size_t count);

void createDescriptorPool(GraphicItems* gi, ShaderDescriptor* descriptors, size_t count) ;

void createDescriptorSets(GraphicItems* gi, localParam* params);

#endif // BINDDESCIPTION_H
