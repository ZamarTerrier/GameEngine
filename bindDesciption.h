#ifndef BINDDESCIPTION_H
#define BINDDESCIPTION_H

#include "stdinclude.h"

#include "resource.h"

VkVertexInputBindingDescription getBindingDescription();

attrDescr getAttributeDescriptions();

void createDescriptorSetLayout(void* arg, VkDescriptorType* descriptors, size_t count);

void createDescriptorPool(void* arg, VkDescriptorType* descriptors, size_t count);

void createDescriptorSets(void* arg);

#endif // BINDDESCIPTION_H
