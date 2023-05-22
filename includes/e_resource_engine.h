#ifndef E_RESOURCE_ENGINE_H
#define E_RESOURCE_ENGINE_H

#include "engine_includes.h"

extern bool enableValidationLayers;

extern EIExtent2D swapChainExtent;

extern void* e_window;
extern void* instance;
extern void* debugMessenger;

extern void* e_physicalDevice;
extern void* e_device;

extern void* graphicsQueue;
extern void* presentQueue;

extern void* surface;

extern void *current_render;

extern void* swapChain;
extern void** swapChainImages;
extern void** swapChainImageViews;

extern void* depthImage;
extern void* depthImageMemory;
extern void* depthImageView;

extern void* renderPass;
extern void* commandPool;

extern void** swapChainFramebuffers;
extern void** commandBuffers;

extern void** imageAvailableSemaphores;
extern void** renderFinishedSemaphores;

extern void** inFlightFences;
extern void** imagesInFlight;

extern size_t currentFrame;

extern uint32_t swapChainImageFormat;

extern uint32_t MAX_FRAMES_IN_FLIGHT;

extern uint32_t imageIndex;

extern uint32_t extensionCount;
extern uint32_t imagesCount;
extern uint32_t glfwExtensionCount;

extern bool framebufferResized;
extern bool framebufferwasResized;

extern ChildStack *alloc_buffers_memory_head;
extern ChildStack *alloc_descriptor_head;
extern ChildStack *alloc_pipeline_head;

#endif // E_RESOURCE_ENGINE_H
