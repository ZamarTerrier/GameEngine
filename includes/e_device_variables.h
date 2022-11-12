#ifndef E_DEVICE_VARIABLES_H
#define E_DEVICE_VARIABLES_H

#include "engine_includes.h"

#include <stdint.h>

typedef struct{
    uint32_t graphicsFamily;
    uint32_t presentFamily;
} QueueFamilyIndices;

typedef struct EDSurfaceFormatKHR {
    uint32_t           format;
    uint32_t           colorSpace;
} EDSurfaceFormatKHR;

typedef struct{
    EISurfaceCapabilitiesKHR capabilities;
    EDSurfaceFormatKHR* formats;
    uint32_t sizeFormats;
    uint32_t* presentModes;
    uint32_t sizeModes;
} SwapChainSupportDetails;

#endif // E_DEVICE_VARIABLES_H
