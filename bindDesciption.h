#ifndef BINDDESCIPTION_H
#define BINDDESCIPTION_H

#include "stdinclude.h"
#include "graphicsItems.h"

#include "e_resource.h"

VkVertexInputBindingDescription getBindingDescription();

attrDescr getAttributeDescriptions();

void createDescriptorSetLayout(GraphicItems* gi, ShaderBuffer* descriptors, size_t count);

void createDescriptorPool(GraphicItems* gi, ShaderBuffer* descriptors, size_t count) ;

void createDescriptorSets(GraphicItems* gi, localParam* params);

#endif // BINDDESCIPTION_H
