#include "e_window.h"

#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

#include "e_resource_data.h"
#include "e_resource_engine.h"

bool checkValidationLayerSupport(){
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, NULL);

    VkLayerProperties* availableLayers = (VkLayerProperties*) malloc(layerCount * sizeof(VkLayerProperties));
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers);

    bool layerFound;

    for(int i=0; i < num_valid_layers; i++)
    {
        layerFound = false;

        for(int j=0; j < layerCount; j++)
        {
            if(strcmp(validationLayers[i], availableLayers[j].layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if(!layerFound)
            return false;
    }

    free(availableLayers);

    return true;

}

const char** getRequiredExtensions(){

    const char** extensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    const char** glfwExtensions = (const char **) calloc(glfwExtensionCount, sizeof(char *));

    memcpy(glfwExtensions, extensions, sizeof(char *) * glfwExtensionCount);

    if(enableValidationLayers){
        glfwExtensionCount ++;
        realloc(glfwExtensions, sizeof(const char*) * glfwExtensionCount);
        glfwExtensions[glfwExtensionCount - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    }

    return glfwExtensions;

}

void initWindow(){

    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API,  GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    e_window = glfwCreateWindow(WIDTH, HEIGHT, app_name, NULL, NULL);

    glfwSetFramebufferSizeCallback(e_window, framebufferResizeCallback);

}

static void framebufferResizeCallback(void* window, int width, int height) {

    if(framebufferwasResized)
        framebufferResized = true;

}

void createInstance(){

    if(enableValidationLayers && !checkValidationLayerSupport()){
        printf("validation layers requested, but not available\n");
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Test";
    appInfo.applicationVersion = VK_MAKE_VERSION(0,0,0);
    appInfo.pEngineName = "My Engine";
    appInfo.applicationVersion = VK_MAKE_VERSION(0,0,0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;


    VkDebugUtilsMessengerCreateInfoEXT debugInfo;
    if(enableValidationLayers){
        createInfo.enabledLayerCount = num_valid_layers;
        createInfo.ppEnabledLayerNames = validationLayers;

        populateDebugMessengerCreateInfo(&debugInfo);

        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugInfo;
    } else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = NULL;
    }

    const char** glfwExtensions = getRequiredExtensions();

    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    if(vkCreateInstance(&createInfo, NULL, &instance)){
        printf("Failed to create instance\n");
        exit(1);
    }

    free(glfwExtensions);
}

void createSurface() {
    if (glfwCreateWindowSurface(instance, e_window, NULL, &surface) != VK_SUCCESS) {
        printf("failed to create window surface!");
        exit(1);
    }
}

vec2 getWindowSize()
{
    vec2 size;

    size.x = WIDTH * diffSize.x;
    size.y = HEIGHT * diffSize.y;

    return size;
}
