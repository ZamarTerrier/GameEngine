#ifndef E_DESCRIPTOR_H
#define E_DESCRIPTOR_H

#include "engine_includes.h"
#include "graphicsItems.h"

#include "e_blue_print.h"

void DescriptorUpdate(Blueprints *blueprints, uint32_t indx_pack, uint32_t indx_descr, char *data, uint32_t size_data);
void DescriptorCreate(ShaderDescriptor *descriptor, BluePrintDescriptor* descriptors,  Blueprints *blueprints, size_t num_descr, size_t num_frame);

#endif // E_DESCRIPTOR_H
