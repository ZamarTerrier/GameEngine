#include "e_resource.h"

#include <vulkan/vulkan.h>

uint32_t MAX_FRAMES_IN_FLIGHT = 2;
int WIDTH = 1024;
int HEIGHT = 800;
uint32_t VALIDSIZE = 1;
uint32_t DEVEXTSIZE = 1;

char app_name[256];

bool enableValidationLayers = true;

void* e_window;
void* instance;
void* debugMessenger;

void* physicalDevice;
void* device;

void* graphicsQueue;
void* presentQueue;

void* surface;

void* swapChain;
void** swapChainImages;
void** swapChainImageViews;

void* depthImage;
void* depthImageMemory;
void* depthImageView;

uint32_t swapChainImageFormat;
EIExtent2D swapChainExtent;

void* renderPass;

void** swapChainFramebuffers;

void* commandPool;
void** commandBuffers;

void** imageAvailableSemaphores;
void** renderFinishedSemaphores;

void** inFlightFences;
void** imagesInFlight;
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
    {{-0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}},
    {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}},
    {{0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}},
    {{0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}},
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

EIVertexInputAttributeDescription planeAttributeDescription[] = {

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

EIVertexInputAttributeDescription cubeAttributeDescription[] = {

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

EIVertexInputAttributeDescription modelAttributeDescription[] = {
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

EIVertexInputAttributeDescription particle2DAttributeDescription[] = {
    {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(ParticleVertex2D, position) },
    {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32_SFLOAT,
        .offset = offsetof(ParticleVertex2D, size) },
    {
        .binding = 0,
        .location = 2,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(ParticleVertex2D, color) }
};

EIVertexInputAttributeDescription particle3DAttributeDescription[] = {
    {
        .binding = 0,
        .location = 0,
        .format = VK_FORMAT_R32G32_SFLOAT,
        .offset = offsetof(ParticleVertex3D, position) },
    {
        .binding = 0,
        .location = 1,
        .format = VK_FORMAT_R32_SFLOAT,
        .offset = offsetof(ParticleVertex3D, size) },
    {
        .binding = 0,
        .location = 2,
        .format = VK_FORMAT_R32G32B32_SFLOAT,
        .offset = offsetof(ParticleVertex3D, color) }
};


const FontIndexes fontIndexes[] ={
    { 1040, 0xD090 },//А
    { 1041, 0xD091 },//Б
    { 1042, 0xD092 },//В
    { 1043, 0xD093 },//Г
    { 1044, 0xD094 },//Д
    { 1045, 0xD095 },//Е
    { 1046, 0xD096 },//Ж
    { 1047, 0xD097 },//З
    { 1048, 0xD098 },//И
    { 1049, 0xD099 },//Й
    { 1050, 0xD09A },//К
    { 1051, 0xD09B },//Л
    { 1052, 0xD09C },//М
    { 1053, 0xD09D },//Н
    { 1054, 0xD09E },//О
    { 1055, 0xD09F },//П
    { 1056, 0xD0A0 },//Р
    { 1057, 0xD0A1 },//С
    { 1058, 0xD0A2 },//Т
    { 1059, 0xD0A3 },//У
    { 1060, 0xD0A4 },//Ф
    { 1061, 0xD0A5 },//Х
    { 1062, 0xD0A6 },//Ц
    { 1063, 0xD0A7 },//Ч
    { 1064, 0xD0A8 },//Ш
    { 1065, 0xD0A9 },//Щ
    { 1066, 0xD0AA },//Ъ
    { 1067, 0xD0AB },//Ы
    { 1068, 0xD0AC },//Ь
    { 1069, 0xD0AD },//Э
    { 1070, 0xD0AE },//Ю
    { 1071, 0xD0AF },//Я
    { 1072, 0xD0B0 },//а
    { 1073, 0xD0B1 },//б
    { 1074, 0xD0B2 },//в
    { 1075, 0xD0B3 },//г
    { 1076, 0xD0B4 },//д
    { 1077, 0xD0B5 },//е
    { 1078, 0xD0B6 },//ж
    { 1079, 0xD0B7 },//з
    { 1080, 0xD0B8 },//и
    { 1081, 0xD0B9 },//й
    { 1082, 0xD0BA },//к
    { 1083, 0xD0BB },//л
    { 1084, 0xD0BC },//м
    { 1085, 0xD0BD },//н
    { 1086, 0xD0BE },//о
    { 1087, 0xD0BF },//п
    { 1088, 0xD180 },//р
    { 1089, 0xD181 },//с
    { 1090, 0xD182 },//т
    { 1091, 0xD183 },//у
    { 1092, 0xD184 },//ф
    { 1093, 0xD185 },//х
    { 1094, 0xD186 },//ц
    { 1095, 0xD187 },//ч
    { 1096, 0xD188 },//ш
    { 1097, 0xD189 },//щ
    { 1098, 0xD18A },//ъ
    { 1099, 0xD18B },//ы
    { 1100, 0xD18C },//ь
    { 1101, 0xD18D },//э
    { 1102, 0xD18E },//ю
    { 1103, 0xD18F },//я
};

/*
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
};*/

uint32_t imageIndex;
uint32_t extensionCount = 0;
uint32_t imagesCount = 0;
uint32_t glfwExtensionCount = 0;

void* cam2D;
void* cam3D;

void *e_var_current_entry = NULL;

vec2 viewSize = { 800, 600 };
vec2 diffSize = { 1.0f, 1.0f };

char *rootDirPath;

void *e_var_images;
int e_var_num_images = 0;

void **e_var_lights = NULL;
int e_var_num_lights = 0;
