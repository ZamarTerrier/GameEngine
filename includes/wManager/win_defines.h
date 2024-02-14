#ifndef WIN_DEFINES_H
#define WIN_DEFINES_H

#if defined(_WIN32)
 #define APIENTRY __stdcall
#else
 #define APIENTRY
#endif

#ifdef _WIN_
    #include <wManager/manager_includes.h>

    #include <windows.h>

    #include <vulkan/vulkan.h>

    #include <stdint.h>
    #include <stdbool.h>

    typedef struct{
        HWND handle;
        MSG message;


        ATOM mainWindowClass;
        HINSTANCE instance;

        char*               clipboardString;
        short int           keycodes[512];
        short int           scancodes[ENGINE_KEY_LAST + 1];
        char                keynames[ENGINE_KEY_LAST + 1][5];

        // The last received high surrogate when decoding pairs of UTF-16 messages
        WCHAR               highSurrogate;

        RAWINPUT*           rawInput;
        int                 rawInputSize;        

        double restoreCursorPosX, restoreCursorPosY;
        double virtualCursorPosX, virtualCursorPosY;
        double lastCursorPosX , lastCursorPosY;

        struct wManagerWindow* capturedCursorWindow;
        struct wManagerWindow* disabledCursorWindow;
    } wManagerWin;

    typedef VkFlags VkWin32SurfaceCreateFlagsKHR;

    typedef struct VkWin32SurfaceCreateInfoKHR
    {
        VkStructureType                 sType;
        const void*                     pNext;
        VkWin32SurfaceCreateFlagsKHR    flags;
        HINSTANCE                       hinstance;
        HWND                            hwnd;
    } VkWin32SurfaceCreateInfoKHR;

    typedef VkResult (APIENTRY *PFN_vkCreateWin32SurfaceKHR)(VkInstance,const VkWin32SurfaceCreateInfoKHR*,const VkAllocationCallbacks*,VkSurfaceKHR*);
    typedef VkBool32 (APIENTRY *PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)(VkPhysicalDevice,uint32_t);

#endif

#endif // WIN_DEFINES_H
