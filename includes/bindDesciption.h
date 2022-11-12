#ifndef BINDDESCIPTION_H
#define BINDDESCIPTION_H

#include "engine_includes.h"
#include "graphicsItems.h"

#include "e_resource.h"

void createDescriptorSetLayout(GraphicItems* gi, ShaderBuffer* descriptors, size_t count);

void createDescriptorPool(GraphicItems* gi, ShaderBuffer* descriptors, size_t count) ;

void createDescriptorSets(GraphicItems* gi, localParam* params);

#endif // BINDDESCIPTION_H
