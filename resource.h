#ifndef RECOURCE_H
#define RECOURCE_H

#include "stdinclude.h"

extern uint32_t MAX_FRAMES_IN_FLIGHT;
extern uint32_t WIDTH;
extern uint32_t HEIGHT;
extern uint32_t VALIDSIZE;
extern uint32_t DEVEXTSIZE;

extern char app_name[256];

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

extern const Vertex3D triVert[];
extern const Vertex3D quadVert[];
extern const Vertex2D planeVert[];
extern const Vertex3D cubeVert[];

extern const uint16_t triIndx[];
extern const uint16_t quadIndx[];
extern const uint16_t cubeIndx[];
extern const uint16_t planeIndx[];

extern const char* validationLayers[];
extern const char* deviceExtensions[];

extern VkVertexInputAttributeDescription planeAttributeDescription[];
extern VkVertexInputAttributeDescription cubeAttributeDescription[];

typedef struct{
    int FindLetter;
    int IndexLetter;
} FontIndexes;

extern const FontIndexes fontIndexes[];

extern uint32_t imageIndex;
extern uint32_t extensionCount;
extern uint32_t imagesCount;
extern uint32_t glfwExtensionCount;

extern void* cam2D;
extern void* cam3D;

extern vec2 viewSize;
extern vec2 diffSize;

extern int i, j;

#endif
