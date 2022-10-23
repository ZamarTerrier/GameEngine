#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include "stdinclude.h"

#include "e_resource.h"
#include "texture.h"
#include "tools.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

double clamp(double x, double upper, double lower);

void querySwapChainSupport(VkPhysicalDevice device, SwapChainSupportDetails* details) ;

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const VkSurfaceFormatKHR* availableFormats, uint32_t sizeFormats);

VkPresentModeKHR chooseSwapPresentMode(const VkPresentModeKHR* availablePresentModes, uint32_t sizeModes);

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR capabilities);

void createSwapChain();

void createImageViews();

#endif // SWAPCHAIN_H
