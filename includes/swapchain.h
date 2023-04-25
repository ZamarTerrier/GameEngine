#ifndef SWAPCHAIN_H
#define SWAPCHAIN_H

#include "engine_includes.h"

#include "e_texture.h"
#include "e_tools.h"

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

void SwapChainCreate();

void SwapChainCreateImageViews();

#ifdef __cplusplus
}
#endif

#endif // SWAPCHAIN_H
