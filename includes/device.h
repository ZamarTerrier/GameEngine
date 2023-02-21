#ifndef DEVICE_H
#define DEVICE_H

#include "engine_includes.h"

#include "swapchain.h"
#include "tools.h"

bool checkDeviceExtensionSupport(void* device);

bool isDeviceSuitable(void* device);

void pickPhysicalDevice();

void createLogicalDevice();

#endif
