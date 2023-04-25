#ifndef E_DEVICE_H
#define E_DEVICE_H

#include "engine_includes.h"

#include "swapchain.h"
#include "e_tools.h"

bool checkDeviceExtensionSupport(void* device);

bool isDeviceSuitable(void* device);

void pickPhysicalDevice();

void createLogicalDevice();

#endif
