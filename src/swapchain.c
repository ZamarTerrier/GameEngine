#include "swapchain.h"

#include <vulkan/vulkan.h>

#include "e_device_variables.h"

#include "e_resource_data.h"
#include "e_resource_engine.h"

void querySwapChainSupport(void* arg, SwapChainSupportDetails* details) {

    VkPhysicalDevice *device = arg;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details->capabilities);

    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details->sizeFormats, NULL);

    if (details->sizeFormats != 0) {
        details->formats = (VkSurfaceFormatKHR* ) calloc(details->sizeFormats, sizeof(VkSurfaceFormatKHR));
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &details->sizeFormats, details->formats);
    }

    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details->sizeModes, NULL);

    if (details->sizeModes != 0) {
        details->presentModes = (VkPresentModeKHR*) calloc(details->sizeModes, sizeof(VkPresentModeKHR));
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &details->sizeModes, details->presentModes);
    }

}

EDSurfaceFormatKHR chooseSwapSurfaceFormat(const EDSurfaceFormatKHR* availableFormats, uint32_t sizeFormats) {


    for (int i=0; i < sizeFormats;i++) {
        if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormats[i];
        }
    }

    return availableFormats[0];
}

uint32_t chooseSwapPresentMode(const void** arg, uint32_t sizeModes) {

    VkPresentModeKHR* availablePresentModes = arg;

    for (int i=0; i < sizeModes;i++) {
        if (availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentModes[i];
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

EIExtent2D chooseSwapExtent(const EISurfaceCapabilitiesKHR capabilities) {

    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {

        VkExtent2D actualExtent = { WIDTH, HEIGHT };

        actualExtent.width = clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return *(EIExtent2D*)&actualExtent;
    }
}

void SwapChainCreate() {

    SwapChainSupportDetails swapChainSupport;

    querySwapChainSupport(e_physicalDevice, &swapChainSupport);

    EDSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats, swapChainSupport.sizeFormats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes, swapChainSupport.sizeModes);
    EIExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    imagesCount = swapChainSupport.capabilities.minImageCount + 1;

    if (swapChainSupport.capabilities.maxImageCount > 0 && imagesCount > swapChainSupport.capabilities.maxImageCount) {
        imagesCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;

    createInfo.minImageCount = imagesCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = *(VkExtent2D*)&extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(e_physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily, indices.presentFamily};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(e_device, &createInfo, NULL, &swapChain) != VK_SUCCESS) {
        printf("failed to create swap chain!");
        exit(1);
    }

    vkGetSwapchainImagesKHR(e_device, swapChain, &imagesCount, NULL);
    swapChainImages = (VkImage *) calloc(imagesCount, sizeof(VkImage));
    vkGetSwapchainImagesKHR(e_device, swapChain, &imagesCount, swapChainImages);

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = *(EIExtent2D*)&extent;

}

void SwapChainCreateImageViews() {

    swapChainImageViews = (VkImageView *) calloc(imagesCount, sizeof(VkImageView));

    for (size_t i = 0; i < imagesCount; i++) {
        swapChainImageViews[i] = TextureCreateImageView(swapChainImages[i], VK_IMAGE_VIEW_TYPE_2D, swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }

}
