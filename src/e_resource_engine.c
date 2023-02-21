#include "e_resource_engine.h"

bool enableValidationLayers = true;

EIExtent2D swapChainExtent;

void* e_window;
void* instance;
void* debugMessenger;

void* e_physicalDevice;
void* e_device;

void* graphicsQueue;
void* presentQueue;

void* surface;

void* swapChain;
void** swapChainImages;
void** swapChainImageViews;

void* depthImage;
void* depthImageMemory;
void* depthImageView;

void* renderPass;
void* commandPool;

void** swapChainFramebuffers;
void** commandBuffers;

void** imageAvailableSemaphores;
void** renderFinishedSemaphores;

void** inFlightFences;
void** imagesInFlight;

size_t currentFrame = 0;

uint32_t swapChainImageFormat;

uint32_t MAX_FRAMES_IN_FLIGHT = 2;

uint32_t imageIndex;

uint32_t extensionCount = 0;
uint32_t imagesCount = 0;
uint32_t glfwExtensionCount = 0;

bool framebufferResized = false;
bool framebufferwasResized = true;
