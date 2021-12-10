#ifndef RECOURCE_H
#define RECOURCE_H

#include "stdinclude.h"

extern uint32_t MAX_FRAMES_IN_FLIGHT;
extern uint32_t WIDTH;
extern uint32_t HEIGHT;
extern uint32_t VALIDSIZE;
extern uint32_t DEVEXTSIZE;

extern bool enableValidationLayers;

extern GLFWwindow* window;
extern VkInstance instance;
extern VkDebugUtilsMessengerEXT debugMessenger;

extern VkPhysicalDevice physicalDevice;
extern VkDevice device;

extern VkQueue graphicsQueue;
extern VkQueue presentQueue;

extern VkSurfaceKHR surface;

extern VkSwapchainKHR swapChain;
extern VkImage* swapChainImages;
extern VkImageView* swapChainImageViews;

extern VkImage depthImage;
extern VkDeviceMemory depthImageMemory;
extern VkImageView depthImageView;

extern VkFormat swapChainImageFormat;
extern VkExtent2D swapChainExtent;

extern VkRenderPass renderPass;

extern VkFramebuffer* swapChainFramebuffers;

extern VkCommandPool commandPool;
extern VkCommandBuffer* commandBuffers;

extern VkSemaphore* imageAvailableSemaphores;
extern VkSemaphore* renderFinishedSemaphores;

extern VkFence* inFlightFences;
extern VkFence* imagesInFlight;
extern size_t currentFrame;

extern bool framebufferResized;
extern bool framebufferwasResized;

extern VkBuffer vertexBuffer;
extern VkDeviceMemory vertexBufferMemory;
extern VkBuffer indexBuffer;
extern VkDeviceMemory indexBufferMemory;

extern VkBuffer* uniformBuffers;
extern VkDeviceMemory* uniformBuffersMemory;

extern const Vertex vertices[];
extern const uint16_t indices[];

extern const char* validationLayers[];
extern const char* deviceExtensions[];

extern uint32_t imageIndex;
extern uint32_t extensionCount;
extern uint32_t imagesCount;
extern uint32_t glfwExtensionCount;

extern void* camObj;

extern vec2 viewSize;
extern vec2 diffSize;

extern int i, j;

#endif
