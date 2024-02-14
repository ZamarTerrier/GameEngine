#ifndef E_DEVICE_H
#define E_DEVICE_H

#include "Variabels/engine_includes.h"

#include "swapchain.h"
#include "Tools/e_tools.h"

bool checkDeviceExtensionSupport(void* device);

bool isDeviceSuitable(void* device);

void pickPhysicalDevice();

void createLogicalDevice();

#endif
