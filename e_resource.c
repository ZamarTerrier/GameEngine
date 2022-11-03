#include "e_resource.h"

uint32_t MAX_FRAMES_IN_FLIGHT = 2;
uint32_t WIDTH = 1024;
uint32_t HEIGHT = 800;
uint32_t VALIDSIZE = 1;
uint32_t DEVEXTSIZE = 1;

char app_name[256];

bool enableValidationLayers = true;

GLFWwindow* e_window;
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

const Vertex3D lineVert[] = {
    {{0, 0.0f, 0}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{0, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
};

const Vertex3D triVert[] = {
    {{-1.0f, -1.0f, 0}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{1.0f, -1.0f, 0}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.0f, 1.0f, 0}, {0.0f,-1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
};

const Vertex3D quadVert[] = {
    {{-1.0f, -1.0f, 0}, {-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{-1.0f, 1.0f, 0}, {-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{1.0f, 1.0f, 0}, {1.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{1.0f, -1.0f, 0}, {1.0f, -1.0f, 0.0f}, {0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
};

const Vertex2D planeVert[] = {
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{-0.5f, 0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{0.5f, -0.5f}, {0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
};

const Vertex2D projPlaneVert[] = {
    {{-1.0f, -1.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
    {{-1.0f,  1.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
    {{ 1.0f,  1.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
    {{ 1.0f, -1.0f}, {0.5f, 0.5f, 0.5f}, {1.0f, 0.0f}},
};

const uint32_t triIndx[] = {
    0, 1, 2
};

const uint32_t quadIndx[] = {
    0, 1, 2, 2, 3, 0
};

const uint32_t planeIndx[] = {
    2, 1, 0, 0, 3, 2
};

const uint32_t projPlaneIndx[] = {
    2, 1, 0, 0, 3, 2
};

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
        .offset = offsetof(Vertex3D, normal) },
    {
        .binding = 0,
        .location = 2,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(Vertex3D, color) },
    {
        .binding = 0,
        .location = 3,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(Vertex3D, texCoord) }
};

VkVertexInputAttributeDescription modelAttributeDescription[] = {
    {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(ModelVertex3D, position) },
    {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(ModelVertex3D, normal) },
    {
        .binding = 0,
        .location = 2,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(ModelVertex3D, color) },
    {
        .binding = 0,
        .location = 3,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(ModelVertex3D, texCoord) },
    {
        .binding = 0,
        .location = 4,
        .format = VK_FORMAT_R32G32B32A32_SFLOAT,
        .offset = offsetof(ModelVertex3D, joints) },
    {
        .binding = 0,
        .location = 5,
        .format = VK_FORMAT_R32G32B32A32_SFLOAT,
        .offset = offsetof(ModelVertex3D, weight) }
};

const FontIndexes fontIndexes[] ={
    { 1040, 225 },//А
    { 1041, 226 },//Б
    { 1042, 247 },//В
    { 1043, 231 },//Г
    { 1044, 228 },//Д
    { 1045, 229 },//Е
    { 1046, 246 },//Ж
    { 1047, 250 },//З
    { 1048, 233 },//И
    { 1049, 234 },//Й
    { 1050, 235 },//К
    { 1051, 236 },//Л
    { 1052, 237 },//М
    { 1053, 238 },//Н
    { 1054, 239 },//О
    { 1055, 240 },//П
    { 1056, 242 },//Р
    { 1057, 243 },//С
    { 1058, 244 },//Т
    { 1059, 245 },//У
    { 1060, 230 },//Ф
    { 1061, 232 },//Х
    { 1062, 227 },//Ц
    { 1063, 254 },//Ч
    { 1064, 251 },//Ш
    { 1065, 253 },//Щ
    { 1066, 255 },//Ъ
    { 1067, 249 },//Ы
    { 1068, 248 },//Ь
    { 1069, 252 },//Э
    { 1070, 224 },//Ю
    { 1071, 241 },//Я
    { 1072, 193 },//а
    { 1073, 194 },//б
    { 1074, 215 },//в
    { 1075, 199 },//г
    { 1076, 196 },//д
    { 1077, 197 },//е
    { 1078, 214 },//ж
    { 1079, 218 },//з
    { 1080, 201 },//и
    { 1081, 202 },//й
    { 1082, 203 },//к
    { 1083, 204 },//л
    { 1084, 205 },//м
    { 1085, 206 },//н
    { 1086, 207 },//о
    { 1087, 208 },//п
    { 1088, 210 },//р
    { 1089, 211 },//с
    { 1090, 212 },//т
    { 1091, 213 },//у
    { 1092, 198 },//ф
    { 1093, 200 },//х
    { 1094, 195 },//ц
    { 1095, 222 },//ч
    { 1096, 219 },//ш
    { 1097, 221 },//щ
    { 1098, 223 },//ъ
    { 1099, 217 },//ы
    { 1100, 216 },//ь
    { 1101, 220 },//э
    { 1102, 192 },//ю
    { 1103, 209 },//я
};

uint32_t imageIndex;
uint32_t extensionCount = 0;
uint32_t imagesCount = 0;
uint32_t glfwExtensionCount = 0;

void* cam2D;
void* cam3D;

void *current_entry = NULL;

vec2 viewSize = { 800, 600 };
vec2 diffSize = { 1.0f, 1.0f };

int indSphereCount = 0, vertSphereCount = 0;
int indPlaneCount = 0, vertPlaneCount = 0;
int indConeCount = 0, vertConeCount = 0;

char *rootDirPath;
