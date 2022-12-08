#include "debuger.h"

#include <vulkan/vulkan.h>

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

uint32_t CreateDebugUtilsMessengerEXT(void* arg, const EdDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const EdAllocationCallbacks* pAllocator, void** messenger) {

    VkInstance *instance = arg;
    VkDebugUtilsMessengerEXT* pDebugMessenger = messenger;

    int (*func1)(VkInstance, const VkDebugUtilsMessengerCreateInfoEXT*, const EdAllocationCallbacks*, VkDebugUtilsMessengerEXT*);

    func1 = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func1 != NULL) {
        return func1(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void DestroyDebugUtilsMessengerEXT(void* arg, void* debugMessenger, const EdAllocationCallbacks* pAllocator) {

    VkInstance *instance = arg;

    int (*func2)(VkInstance, VkDebugUtilsMessengerEXT, const VkAllocationCallbacks*);

    func2 = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func2 != NULL) {
        func2(instance, debugMessenger, pAllocator);
    }
}


void *thread_f(const char *text) //функция для вычисления элемента матрицы
{
    printf("validation layer : %s\n", text);
}

uint32_t debugCallback(
        uint32_t messageSeverity,
        uint32_t messageType,
        const void** CallbackData,
        void* pUserData){

    VkDebugUtilsMessengerCallbackDataEXT *pCallbackData = CallbackData;

    if(messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        int i =0;
    }

    pthread_t threader;
    pthread_create(&threader, NULL, thread_f, (void *)pCallbackData->pMessage);
    //переводим в отсоединенное состояние
    pthread_detach(threader);

    return VK_FALSE;

}

void populateDebugMessengerCreateInfo(EdDebugUtilsMessengerCreateInfoEXT* createInfo) {
    createInfo->sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo->messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo->messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo->pfnUserCallback = debugCallback;
}

void setupDebugMessenger(){

    if(!enableValidationLayers) return;

    VkDebugUtilsMessengerCreateInfoEXT debugInfo;
    populateDebugMessengerCreateInfo(&debugInfo);
    if(CreateDebugUtilsMessengerEXT(instance, &debugInfo, NULL, &debugMessenger) != VK_SUCCESS)
    {
        printf("failed create debug messnger");
        exit(1);
    }

}
