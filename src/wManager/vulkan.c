#include "wManager/manager_includes.h"
#include "wManager/window_manager.h"

#include <vulkan/vulkan.h>

#include <stdbool.h>

wManagerInfo _wMInfo;
extern wManagerWindow _wMWindow;

const char* _wManagerGetVulkanResultString(VkResult result)
{
    switch (result)
    {
        case VK_SUCCESS:
            return "Success";
        case VK_NOT_READY:
            return "A fence or query has not yet completed";
        case VK_TIMEOUT:
            return "A wait operation has not completed in the specified time";
        case VK_EVENT_SET:
            return "An event is signaled";
        case VK_EVENT_RESET:
            return "An event is unsignaled";
        case VK_INCOMPLETE:
            return "A return array was too small for the result";
        case VK_ERROR_OUT_OF_HOST_MEMORY:
            return "A host memory allocation has failed";
        case VK_ERROR_OUT_OF_DEVICE_MEMORY:
            return "A device memory allocation has failed";
        case VK_ERROR_INITIALIZATION_FAILED:
            return "Initialization of an object could not be completed for implementation-specific reasons";
        case VK_ERROR_DEVICE_LOST:
            return "The logical or physical device has been lost";
        case VK_ERROR_MEMORY_MAP_FAILED:
            return "Mapping of a memory object has failed";
        case VK_ERROR_LAYER_NOT_PRESENT:
            return "A requested layer is not present or could not be loaded";
        case VK_ERROR_EXTENSION_NOT_PRESENT:
            return "A requested extension is not supported";
        case VK_ERROR_FEATURE_NOT_PRESENT:
            return "A requested feature is not supported";
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            return "The requested version of Vulkan is not supported by the driver or is otherwise incompatible";
        case VK_ERROR_TOO_MANY_OBJECTS:
            return "Too many objects of the type have already been created";
        case VK_ERROR_FORMAT_NOT_SUPPORTED:
            return "A requested format is not supported on this device";
        case VK_ERROR_SURFACE_LOST_KHR:
            return "A surface is no longer available";
        case VK_SUBOPTIMAL_KHR:
            return "A swapchain no longer matches the surface properties exactly, but can still be used";
        case VK_ERROR_OUT_OF_DATE_KHR:
            return "A surface has changed in such a way that it is no longer compatible with the swapchain";
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
            return "The display used by a swapchain does not use the same presentable image layout";
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
            return "The requested window is already connected to a VkSurfaceKHR, or to some other non-Vulkan API";
        case VK_ERROR_VALIDATION_FAILED_EXT:
            return "A validation layer found an error";
        default:
            return "ERROR: UNKNOWN VULKAN ERROR";
    }
}

int wManagerVulkanInit()
{
        VkResult err;
        VkExtensionProperties* ep;
        PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties;
        uint32_t i, count;

        if (_wMInfo.vk.available)
            return true;

        vkEnumerateInstanceExtensionProperties = (PFN_vkEnumerateInstanceExtensionProperties)
            vkGetInstanceProcAddr(NULL, "vkEnumerateInstanceExtensionProperties");
        if (!vkEnumerateInstanceExtensionProperties)
        {
            printf("Vulkan: Failed to retrieve vkEnumerateInstanceExtensionProperties\n");

            //_wMInfoTerminateVulkan();

            return false;
        }

        err = vkEnumerateInstanceExtensionProperties(NULL, &count, NULL);
        if (err)
        {
            // NOTE: This happens on systems with a loader but without any Vulkan ICD
            /*if (mode == _wMInfo_REQUIRE_LOADER)
            {
                printf("Vulkan: Failed to query instance extension count: %s",
                                    _wMInfoGetVulkanResultString(err));
            }

            //_wMInfoTerminateVulkan();*/
            return false;
        }

        ep = calloc(count, sizeof(VkExtensionProperties));

        err = vkEnumerateInstanceExtensionProperties(NULL, &count, ep);
        if (err)
        {
            printf("Vulkan: Failed to query instance extensions: %s\n",
                            _wManagerGetVulkanResultString(err));

            free(ep);
            //_wMInfoTerminateVulkan();
            return false;
        }

        for (i = 0;  i < count;  i++)
        {
            if (strcmp(ep[i].extensionName, "VK_KHR_surface") == 0)
                _wMInfo.vk.KHR_surface = true;
            else if (strcmp(ep[i].extensionName, "VK_KHR_win32_surface") == 0)
                _wMInfo.vk.KHR_win32_surface = true;
            else if (strcmp(ep[i].extensionName, "VK_MVK_macos_surface") == 0)
                _wMInfo.vk.MVK_macos_surface = true;
            else if (strcmp(ep[i].extensionName, "VK_EXT_metal_surface") == 0)
                _wMInfo.vk.EXT_metal_surface = true;
            else if (strcmp(ep[i].extensionName, "VK_KHR_xlib_surface") == 0)
                _wMInfo.vk.KHR_xlib_surface = true;
            else if (strcmp(ep[i].extensionName, "VK_KHR_xcb_surface") == 0)
                _wMInfo.vk.KHR_xcb_surface = true;
            else if (strcmp(ep[i].extensionName, "VK_KHR_wayland_surface") == 0)
                _wMInfo.vk.KHR_wayland_surface = true;
        }

        free(ep);

        _wMInfo.vk.available = true;

        wManagerInfo *info = &_wMInfo;


        /*if (!_wMInfo.vk.KHR_surface || !_wMInfo.vk.KHR_win32_surface)
            return;

        _wMInfo.vk.extensions[0] = "VK_KHR_surface";
        _wMInfo.vk.extensions[1] = "VK_KHR_win32_surface";*/

        _wMWindow.platform.getRequiredInstanceExtensions(_wMInfo.vk.extensions);

        return true;
}
