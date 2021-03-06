#include "resource.h"

uint32_t MAX_FRAMES_IN_FLIGHT = 2;
uint32_t WIDTH = 1024;
uint32_t HEIGHT = 800;
uint32_t VALIDSIZE = 1;
uint32_t DEVEXTSIZE = 1;

char app_name[256];

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

const Vertex3D triVert[] = {
    {{-0.5f, -0.5f, 0}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{-0.5f, 0.5f, 0}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.5f, 0.5f, 0}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
};

const Vertex3D quadVert[] = {
    {{-0.5f, -0.5f, 0}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
    {{-0.5f, 0.5f, 0}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
    {{0.5f, 0.5f, 0}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
    {{0.5f, -0.5f, 0}, {0.5f, 0.5f, 0.5f}, {1.0f, 1.0f}},
};

const Vertex3D cubeVert[] = {
    {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{-0.5f, 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{0.5f, -0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
    {{-0.5f, -0.5f, 0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{-0.5f, 0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{0.5f, -0.5f, 0.5f}, {0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
};

const Vertex2D planeVert[] = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
};

const uint16_t cubeIndx[] = {
    0, 1, 2, 2, 3, 0,
    7, 6, 5, 5, 4, 7,
    4, 5, 1, 1, 0, 4,
    3, 2, 6, 6, 7, 3,
    2, 1, 5, 5, 6, 2,
    4, 0, 3, 3, 7, 4
};

const uint16_t triIndx[] = {
    0, 1, 2
};

const uint16_t quadIndx[] = {
    0, 1, 2, 2, 3, 0
};

const uint16_t planeIndx[] = {
    2, 1, 0, 0, 3, 2
};

vec2 pos = {0,0};

const char* validationLayers[] = {
    "VK_LAYER_KHRONOS_validation"
};

const char* deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

VkVertexInputAttributeDescription planeAttributeDescription[] = {

    {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(Vertex2D, position) },
    {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex2D, color) },
    {
        .binding = 0,
        .location = 2,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(Vertex2D, texCoord) }
};

VkVertexInputAttributeDescription cubeAttributeDescription[] = {

    {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex3D, position) },
    {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex3D, color) },
    {
        .binding = 0,
        .location = 2,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(Vertex3D, texCoord) }
};

const FontIndexes fontIndexes[] ={
    { 1040, 225 },//??
    { 1041, 226 },//??
    { 1042, 247 },//??
    { 1043, 231 },//??
    { 1044, 228 },//??
    { 1045, 229 },//??
    { 1046, 246 },//??
    { 1047, 250 },//??
    { 1048, 233 },//??
    { 1049, 234 },//??
    { 1050, 235 },//??
    { 1051, 236 },//??
    { 1052, 237 },//??
    { 1053, 238 },//??
    { 1054, 239 },//??
    { 1055, 240 },//??
    { 1056, 242 },//??
    { 1057, 243 },//??
    { 1058, 244 },//??
    { 1059, 245 },//??
    { 1060, 230 },//??
    { 1061, 232 },//??
    { 1062, 227 },//??
    { 1063, 254 },//??
    { 1064, 251 },//??
    { 1065, 253 },//??
    { 1066, 255 },//??
    { 1067, 249 },//??
    { 1068, 248 },//??
    { 1069, 252 },//??
    { 1070, 224 },//??
    { 1071, 241 },//??
    { 1072, 193 },//??
    { 1073, 194 },//??
    { 1074, 215 },//??
    { 1075, 199 },//??
    { 1076, 196 },//??
    { 1077, 197 },//??
    { 1078, 214 },//??
    { 1079, 218 },//??
    { 1080, 201 },//??
    { 1081, 202 },//??
    { 1082, 203 },//??
    { 1083, 204 },//??
    { 1084, 205 },//??
    { 1085, 206 },//??
    { 1086, 207 },//??
    { 1087, 208 },//??
    { 1088, 210 },//??
    { 1089, 211 },//??
    { 1090, 212 },//??
    { 1091, 213 },//??
    { 1092, 198 },//??
    { 1093, 200 },//??
    { 1094, 195 },//??
    { 1095, 222 },//??
    { 1096, 219 },//??
    { 1097, 221 },//??
    { 1098, 223 },//??
    { 1099, 217 },//??
    { 1100, 216 },//??
    { 1101, 220 },//??
    { 1102, 192 },//??
    { 1103, 209 },//??
};

uint32_t imageIndex;
uint32_t extensionCount = 0;
uint32_t imagesCount = 0;
uint32_t glfwExtensionCount = 0;

void* cam2D;
void* cam3D;

vec2 viewSize = { 800, 600 };
vec2 diffSize = { 1.5f, 1.5f };

int i = 0, j = 0;
