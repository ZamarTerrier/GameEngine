#include "Core/e_window.h"

#include "wManager/window_manager.h"

#include <vulkan/vulkan.h>

#include "Data/e_resource_data.h"
#include "Data/e_resource_engine.h"

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

    const char** extensions = wManagerGetRequiredInstanceExtensions(&wManagerExtensionCount);

    if(enableValidationLayers)
        wManagerExtensionCount ++;

    const char** wManagerExtensions = (const char **) calloc(wManagerExtensionCount, sizeof(char *));

    int tempIter = enableValidationLayers ? wManagerExtensionCount - 1 : wManagerExtensionCount;

    for(int i=0; i < tempIter; i++)
        wManagerExtensions[i] = extensions[i];

    if(enableValidationLayers)
        wManagerExtensions[wManagerExtensionCount - 1] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;

    return wManagerExtensions;

}

void initWindow(){

    wManagerInit();

    wManagerWindowHint(ENGINE_RESIZABLE, false);
    if(!wManagerCreateWindow(e_window, WIDTH, HEIGHT, app_name)){
        wManagerTerminate();
        exit(0);
    }

    wManagerSetFramebufferSizeCallback(e_window, framebufferResizeCallback);

}

static void framebufferResizeCallback(void* window, int width, int height) {

    if(framebufferwasResized)
        framebufferResized = true;

}

void createInstance(){

    if(enableValidationLayers && !checkValidationLayerSupport()){
        printf("validation layers requested, but not available\n");
    }

    VkApplicationInfo appInfo;
    memset(&appInfo, 0, sizeof(VkApplicationInfo));

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Test";
    appInfo.applicationVersion = VK_MAKE_VERSION(0,0,0);
    appInfo.pEngineName = "My Engine";
    appInfo.applicationVersion = VK_MAKE_VERSION(0,0,0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo;
    memset(&createInfo, 0, sizeof(VkInstanceCreateInfo));

    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    VkDebugUtilsMessengerCreateInfoEXT debugInfo;
    memset(&debugInfo, 0, sizeof(VkDebugUtilsMessengerCreateInfoEXT));

    if(enableValidationLayers){
        createInfo.enabledLayerCount = num_valid_layers;
        createInfo.ppEnabledLayerNames = validationLayers;

        populateDebugMessengerCreateInfo(&debugInfo);

        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugInfo;
    } else {
        createInfo.enabledLayerCount = 0;

        createInfo.pNext = NULL;
    }

    const char** wManagerExtensions = getRequiredExtensions();

    createInfo.enabledExtensionCount = wManagerExtensionCount;
    createInfo.ppEnabledExtensionNames = wManagerExtensions;

    if(vkCreateInstance(&createInfo, NULL, &instance)){
        printf("Failed to create instance\n");
        exit(1);
    }

    free(wManagerExtensions);
}

void createSurface() {

    wManagerWindow *window = e_window;

    if (wManagerCreateWindowSurface(instance, window, NULL, &surface) != VK_SUCCESS) {
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
