#include "resource.h"

uint32_t MAX_FRAMES_IN_FLIGHT = 2;
uint32_t WIDTH = 800;
uint32_t HEIGHT = 600;
uint32_t VALIDSIZE = 1;
uint32_t DEVEXTSIZE = 1;

bool enableValidationLayers = true;

GLFWwindow* window;
VkInstance instance;
VkDebugUtilsMessengerEXT debugMessenger;

VkPhysicalDevice physicalDevice;
VkDevice device;

VkQueue graphicsQueue;
VkQueue presentQueue;

VkSurfaceKHR surface;

VkSwapchainKHR swapChain;
VkImage* swapChainImages;
VkImageView* swapChainImageViews;

VkImage depthImage;
VkDeviceMemory depthImageMemory;
VkImageView depthImageView;

VkFormat swapChainImageFormat;
VkExtent2D swapChainExtent;

VkRenderPass renderPass;

VkFramebuffer* swapChainFramebuffers;

VkCommandPool commandPool;
VkCommandBuffer* commandBuffers;

VkSemaphore* imageAvailableSemaphores;
VkSemaphore* renderFinishedSemaphores;

VkFence* inFlightFences;
VkFence* imagesInFlight;

size_t currentFrame = 0;

bool framebufferResized = false;
bool framebufferwasResized = true;

const Vertex vertices[] = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
};

const Vertex vertices2[] = {
    {{-0.5f, -0.5f}, {0.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 1.0f}},
    {{0.5f, 0.5f}, {1.0f, 1.0f}},
    {{0.5f, -0.5f}, {1.0f, 0.0f}},
};

const uint16_t indices[] = {
    2, 1, 0, 0, 3, 2
};


vec2 pos = {0,0};

const char* validationLayers[] = {
    "VK_LAYER_KHRONOS_validation"
};

const char* deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

VkVertexInputAttributeDescription attributeDescription[] = {

    {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(Vertex, pos) },
    {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex, color) },
    {
        .binding = 0,
        .location = 2,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(Vertex, texCoord) }
};

uint32_t imageIndex;
uint32_t extensionCount = 0;
uint32_t imagesCount = 0;
uint32_t glfwExtensionCount = 0;

void* camObj;

vec2 viewSize = { 800, 600 };
vec2 diffSize = { 1.5f, 1.5f };

int i = 0, j = 0;
