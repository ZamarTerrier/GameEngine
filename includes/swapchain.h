#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include "stdinclude.h"

#include "e_resource.h"
#include "texture.h"
#include "tools.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void querySwapChainSupport(void* device, SwapChainSupportDetails* details) ;

EDSurfaceFormatKHR chooseSwapSurfaceFormat(const EDSurfaceFormatKHR* availableFormats, uint32_t sizeFormats);

uint32_t chooseSwapPresentMode(const void** availablePresentModes, uint32_t sizeModes);

EIExtent2D chooseSwapExtent(const EISurfaceCapabilitiesKHR capabilities);

void createSwapChain();

void createImageViews();

#endif // SWAPCHAIN_H
