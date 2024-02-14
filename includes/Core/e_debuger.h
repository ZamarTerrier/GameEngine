#ifndef DEBUGGER_H
#define DEBUGGER_H

#include "Variabels/engine_includes.h"

typedef struct EdDebugUtilsMessengerCreateInfoEXT {
    uint32_t                                sType;
    const void*                             pNext;
    uint32_t                                flags;
    uint32_t                                messageSeverity;
    uint32_t                                messageType;
    void*                                   pfnUserCallback;
    void*                                   pUserData;
} EdDebugUtilsMessengerCreateInfoEXT;

typedef struct EdAllocationCallbacks {
    void*                                   pUserData;
    void*                                   pfnAllocation;
    void*                                   pfnReallocation;
    void*                                   pfnFree;
    void*                                   pfnInternalAllocation;
    void*                                   pfnInternalFree;
} EdAllocationCallbacks;


uint32_t CreateDebugUtilsMessengerEXT(void* instance, const EdDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const EdAllocationCallbacks* pAllocator, void** pDebugMessenger);

void DestroyDebugUtilsMessengerEXT(void* instance, void* debugMessenger, const EdAllocationCallbacks* pAllocator);

uint32_t debugCallback(
        uint32_t messageSeverity,
        uint32_t messageType,
        const void** pCallbackData,
        void* pUserData);

void populateDebugMessengerCreateInfo(EdDebugUtilsMessengerCreateInfoEXT* createInfo) ;

void setupDebugMessenger();

#endif
