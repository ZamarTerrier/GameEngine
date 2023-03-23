#ifndef E_DESCRIPTOR_H
#define E_DESCRIPTOR_H

#include "engine_includes.h"
#include "graphicsItems.h"

void DescriptorUpdate(BluePrintDescriptor* descriptors, uint32_t indx, char *data, uint32_t size_data);
void DescriptorCreate(ShaderDescriptor *descriptor, BluePrintDescriptor* descriptors, size_t count, size_t num_frame);

#endif // E_DESCRIPTOR_H
