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
    #include <versionhelpers.h>
    #include <dwmapi.h>

    #include <vulkan/vulkan.h>

    #include <stdint.h>
    #include <stdbool.h>

    #define USER_DEFAULT_SCREEN_DPI 96
    #define EDS_ROTATEDMODE 0x00000004

    #define WGL_NUMBER_PIXEL_FORMATS_ARB 0x2000
    #define WGL_SUPPORT_OPENGL_ARB 0x2010
    #define WGL_DRAW_TO_WINDOW_ARB 0x2001
    #define WGL_PIXEL_TYPE_ARB 0x2013
    #define WGL_TYPE_RGBA_ARB 0x202b
    #define WGL_ACCELERATION_ARB 0x2003
    #define WGL_NO_ACCELERATION_ARB 0x2025
    #define WGL_RED_BITS_ARB 0x2015
    #define WGL_RED_SHIFT_ARB 0x2016
    #define WGL_GREEN_BITS_ARB 0x2017
    #define WGL_GREEN_SHIFT_ARB 0x2018
    #define WGL_BLUE_BITS_ARB 0x2019
    #define WGL_BLUE_SHIFT_ARB 0x201a
    #define WGL_ALPHA_BITS_ARB 0x201b
    #define WGL_ALPHA_SHIFT_ARB 0x201c
    #define WGL_ACCUM_BITS_ARB 0x201d
    #define WGL_ACCUM_RED_BITS_ARB 0x201e
    #define WGL_ACCUM_GREEN_BITS_ARB 0x201f
    #define WGL_ACCUM_BLUE_BITS_ARB 0x2020
    #define WGL_ACCUM_ALPHA_BITS_ARB 0x2021
    #define WGL_DEPTH_BITS_ARB 0x2022
    #define WGL_STENCIL_BITS_ARB 0x2023
    #define WGL_AUX_BUFFERS_ARB 0x2024
    #define WGL_STEREO_ARB 0x2012
    #define WGL_DOUBLE_BUFFER_ARB 0x2011
    #define WGL_SAMPLES_ARB 0x2042
    #define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB 0x20a9
    #define WGL_CONTEXT_DEBUG_BIT_ARB 0x00000001
    #define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x00000002
    #define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126
    #define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
    #define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002
    #define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
    #define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
    #define WGL_CONTEXT_FLAGS_ARB 0x2094
    #define WGL_CONTEXT_ES2_PROFILE_BIT_EXT 0x00000004
    #define WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB 0x00000004
    #define WGL_LOSE_CONTEXT_ON_RESET_ARB 0x8252
    #define WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB 0x8256
    #define WGL_NO_RESET_NOTIFICATION_ARB 0x8261
    #define WGL_CONTEXT_RELEASE_BEHAVIOR_ARB 0x2097
    #define WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB 0
    #define WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB 0x2098
    #define WGL_CONTEXT_OPENGL_NO_ERROR_ARB 0x31b3
    #define WGL_COLORSPACE_EXT 0x309d
    #define WGL_COLORSPACE_SRGB_EXT 0x3089

    #define ERROR_INVALID_VERSION_ARB 0x2095
    #define ERROR_INVALID_PROFILE_ARB 0x2096
    #define ERROR_INCOMPATIBLE_DEVICE_CONTEXTS_ARB 0x2054

    typedef BOOL (WINAPI * PFNWGLSWAPINTERVALEXTPROC)(int);
    typedef BOOL (WINAPI * PFNWGLGETPIXELFORMATATTRIBIVARBPROC)(HDC,int,int,UINT,const int*,int*);
    typedef const char* (WINAPI * PFNWGLGETEXTENSIONSSTRINGEXTPROC)(void);
    typedef const char* (WINAPI * PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC);
    typedef HGLRC (WINAPI * PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC,HGLRC,const int*);

    typedef HGLRC (WINAPI * PFN_wglCreateContext)(HDC);
    typedef BOOL (WINAPI * PFN_wglDeleteContext)(HGLRC);
    typedef PROC (WINAPI * PFN_wglGetProcAddress)(LPCSTR);
    typedef HDC (WINAPI * PFN_wglGetCurrentDC)(void);
    typedef HGLRC (WINAPI * PFN_wglGetCurrentContext)(void);
    typedef BOOL (WINAPI * PFN_wglMakeCurrent)(HDC,HGLRC);
    typedef BOOL (WINAPI * PFN_wglShareLists)(HGLRC,HGLRC);

    #define ENGINE_WIN32_TLS_STATE            _GLFWtlsWin32     win32;
    #define ENGINE_WIN32_MUTEX_STATE          _GLFWmutexWin32   win32;

    typedef struct _wManagerMonitorWin32
    {
        HMONITOR            handle;
        // This size matches the static size of DISPLAY_DEVICE.DeviceName
        WCHAR               adapterName[32];
        WCHAR               displayName[32];
        char                publicAdapterName[32];
        char                publicDisplayName[32];
        int32_t            modesPruned;
        int32_t            modeChanged;
    } _wManagerMonitorWin32;

    // Thread local storage structure
    //
    typedef struct _wManagertls
    {
        int32_t            allocated;
        DWORD               index;
    } _wManagertls;

    // Mutex structure
    //
    typedef struct _wManagermutex
    {
        int32_t            allocated;
        CRITICAL_SECTION    section;
    } _wManagermutex;

    typedef struct _wManagerlibraryWGL
    {

        HDC       dc;
        HGLRC     handle;
        int       interval;

        HINSTANCE                           instance;
        PFN_wglCreateContext                CreateContext;
        PFN_wglDeleteContext                DeleteContext;
        PFN_wglGetProcAddress               GetProcAddress;
        PFN_wglGetCurrentDC                 GetCurrentDC;
        PFN_wglGetCurrentContext            GetCurrentContext;
        PFN_wglMakeCurrent                  MakeCurrent;
        PFN_wglShareLists                   ShareLists;

        PFNWGLSWAPINTERVALEXTPROC           SwapIntervalEXT;
        PFNWGLGETPIXELFORMATATTRIBIVARBPROC GetPixelFormatAttribivARB;
        PFNWGLGETEXTENSIONSSTRINGEXTPROC    GetExtensionsStringEXT;
        PFNWGLGETEXTENSIONSSTRINGARBPROC    GetExtensionsStringARB;
        PFNWGLCREATECONTEXTATTRIBSARBPROC   CreateContextAttribsARB;
        int32_t                            EXT_swap_control;
        int32_t                            EXT_colorspace;
        int32_t                            ARB_multisample;
        int32_t                            ARB_framebuffer_sRGB;
        int32_t                            EXT_framebuffer_sRGB;
        int32_t                            ARB_pixel_format;
        int32_t                            ARB_create_context;
        int32_t                            ARB_create_context_profile;
        int32_t                            EXT_create_context_es2_profile;
        int32_t                            ARB_create_context_robustness;
        int32_t                            ARB_create_context_no_error;
        int32_t                            ARB_context_flush_control;
    } _wManagerlibraryWGL;

    typedef struct{
        HWND handle;
        MSG message;


        HINSTANCE instance;
        HWND helperWindowHandle;
        ATOM helperWindowClass;
        ATOM mainWindowClass;
        HDEVNOTIFY          deviceNotificationHandle;
        int                 acquiredMonitorCount;

        char*               clipboardString;
        short int           keycodes[512];
        short int           scancodes[ENGINE_KEY_LAST + 1];
        char                keynames[ENGINE_KEY_LAST + 1][5];

        // The last received high surrogate when decoding pairs of UTF-16 messages
        WCHAR               highSurrogate;

        RAWINPUT*           rawInput;
        int                 rawInputSize;
        UINT                mouseTrailSize;

        _wManagercontext context;
        _wManagerlibraryWGL wgl;

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
