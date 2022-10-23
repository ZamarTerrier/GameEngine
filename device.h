#ifndef DEVICE_H
#define DEVICE_H

#include "stdinclude.h"

#include "e_resource.h"
#include "swapchain.h"
#include "tools.h"

bool checkDeviceExtensionSupport(VkPhysicalDevice device);

bool isDeviceSuitable(VkPhysicalDevice device);

void pickPhysicalDevice();

void createLogicalDevice();

#endif
