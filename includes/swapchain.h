#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include "engine_includes.h"

#include "texture.h"
#include "tools.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

void querySwapChainSupport(void* device, SwapChainSupportDetails* details) ;

EDSurfaceFormatKHR chooseSwapSurfaceFormat(const EDSurfaceFormatKHR* availableFormats, uint32_t sizeFormats);

uint32_t chooseSwapPresentMode(const void** availablePresentModes, uint32_t sizeModes);

EIExtent2D chooseSwapExtent(const EISurfaceCapabilitiesKHR capabilities);

void createSwapChain();

void createImageViews();

#ifdef __cplusplus
}
#endif

#endif // SWAPCHAIN_H
