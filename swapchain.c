#include "swapchain.h"

double clamp(double x, double upper, double lower)
{
    return MIN(upper, MAX(x, lower));
}

void querySwapChainSupport(VkPhysicalDevice device, SwapChainSupportDetails* details) {

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

VkSurfaceFormatKHR chooseSwapSurfaceFormat(const VkSurfaceFormatKHR* availableFormats, uint32_t sizeFormats) {

    for (i=0; i < sizeFormats;i++) {
        if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB && availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormats[i];
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR chooseSwapPresentMode(const VkPresentModeKHR* availablePresentModes, uint32_t sizeModes) {

    for (i=0; i < sizeModes;i++) {
        if (availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentModes[i];
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR capabilities) {
    if (capabilities.currentExtent.width != UINT32_MAX) {
        return capabilities.currentExtent;
    } else {

        VkExtent2D actualExtent = { WIDTH, HEIGHT };

        actualExtent.width = clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void createSwapChain() {

    SwapChainSupportDetails swapChainSupport;

    querySwapChainSupport(physicalDevice, &swapChainSupport);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats, swapChainSupport.sizeFormats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes, swapChainSupport.sizeModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

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
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
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

    if (vkCreateSwapchainKHR(device, &createInfo, NULL, &swapChain) != VK_SUCCESS) {
        printf("failed to create swap chain!");
        exit(1);
    }

    vkGetSwapchainImagesKHR(device, swapChain, &imagesCount, NULL);
    swapChainImages = (VkImage *) calloc(imagesCount, sizeof(VkImage));
    vkGetSwapchainImagesKHR(device, swapChain, &imagesCount, swapChainImages);

    swapChainImageFormat = surfaceFormat.format;
    swapChainExtent = extent;

}

void createImageViews() {

    swapChainImageViews = (VkImageView *) calloc(imagesCount, sizeof(VkImageView));

    for (size_t i = 0; i < imagesCount; i++) {
        swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
    }

}
