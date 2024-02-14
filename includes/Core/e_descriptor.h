#ifndef E_DESCRIPTOR_H
#define E_DESCRIPTOR_H

#include "Variabels/engine_includes.h"
#include "graphicsItems.h"

#include "e_blue_print.h"

void DescriptorClearAll();
void DescriptorDestroy(ShaderDescriptor *descriptor);
void DescriptorUpdate(BluePrintDescriptor *descriptor, char *data, uint32_t size_data);
void DescriptorUpdateIndex(BluePrintDescriptor *descriptor, char *data, uint32_t size_data, uint32_t index);
void DescriptorCreate(ShaderDescriptor *descriptor, BluePrintDescriptor* descriptors,  Blueprints *blueprints, size_t num_descr, size_t num_frame);

#endif // E_DESCRIPTOR_H
