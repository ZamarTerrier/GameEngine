#include "e_device.h"

#include <vulkan/vulkan.h>

#include "e_resource_data.h"
#include "e_resource_engine.h"

bool checkDeviceExtensionSupport(void* arg) {

    VkPhysicalDevice *device = arg;

    vkEnumerateDeviceExtensionProperties(*device, NULL, &extensionCount, NULL);

    VkExtensionProperties* availableExtensions = (VkExtensionProperties*) calloc( extensionCount, sizeof(VkExtensionProperties));
    vkEnumerateDeviceExtensionProperties(*device, NULL, &extensionCount, availableExtensions);

    const char** requiredExtensions = (const char**) calloc(num_dev_extensions, sizeof(char*));

    for(int i=0;i<extensionCount;i++)
    {
        for(int j=0;j<num_dev_extensions;j++)
        {
            if(strcmp(deviceExtensions[j], availableExtensions[i].extensionName) == 0)
            {
                requiredExtensions[j] = (char *) calloc(strlen(availableExtensions[i].extensionName), sizeof(char));
                memcpy(requiredExtensions[j], deviceExtensions[j], sizeof(char) * strlen(availableExtensions[i].extensionName));
            }
        }
    }

    bool empty = true;

    for(int i=0;i<num_dev_extensions;i++)
    {
        if(requiredExtensions[i] != NULL)
            empty = false;

        free(requiredExtensions[i]);
    }

    free(availableExtensions);
    free(requiredExtensions);


    return !empty;
}

bool isDeviceSuitable(void* arg) {

    VkPhysicalDevice *device = arg;

    QueueFamilyIndices indices = findQueueFamilies(*device);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = true;

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(*device, &supportedFeatures);

    return isComplete(indices) && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy && supportedFeatures.fillModeNonSolid;

}

void pickPhysicalDevice() {

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

    if (deviceCount == 0) {
        printf("failed to find GPUs with Vulkan support!");
        exit(1);
    }

    VkPhysicalDevice* devices = (VkPhysicalDevice *) calloc(deviceCount, sizeof(VkPhysicalDevice));
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices);

    VkPhysicalDevice tDevice;
    for (int i=0; i < deviceCount; i++){
        tDevice = devices[i];
        bool temp = isDeviceSuitable(&tDevice);
        if (temp) {
            e_physicalDevice = tDevice;
            break;
        }
    }

    if(e_physicalDevice == NULL) {
        printf("failed to find a suitable GPU!");
        exit(1);
    }

    free(devices);

}

void createLogicalDevice() {

    QueueFamilyIndices indices = findQueueFamilies(e_physicalDevice);

    VkDeviceQueueCreateInfo* queueCreateInfos;
    queueCreateInfos = (VkDeviceQueueCreateInfo*) calloc(2, sizeof(VkDeviceQueueCreateInfo));

    uint32_t* uniqueQueueFamilies = (uint32_t*) calloc(2, sizeof(uint32_t));
    uniqueQueueFamilies[0] = indices.graphicsFamily;
    uniqueQueueFamilies[1] = indices.presentFamily;

    float queuePriority = 1.0f;
    for (int i=0;i<2;i++) {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = uniqueQueueFamilies[i];
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos[i] = queueCreateInfo;
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = 2;
    createInfo.pQueueCreateInfos = queueCreateInfos;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = num_dev_extensions;
    createInfo.ppEnabledExtensionNames = deviceExtensions;

    if (enableValidationLayers) {
        createInfo.enabledLayerCount = num_valid_layers;
        createInfo.ppEnabledLayerNames = validationLayers;
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(e_physicalDevice, &createInfo, NULL, &e_device) != VK_SUCCESS) {
        printf("failed to create logical device!");
        exit(1);
    }

    vkGetDeviceQueue(e_device, indices.presentFamily, 0, &presentQueue);
    vkGetDeviceQueue(e_device, indices.graphicsFamily, 0, &graphicsQueue);

    free(queueCreateInfos);
    free(uniqueQueueFamilies);
}
