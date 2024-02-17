#ifndef MANAGER_INCLUDES_H
#define MANAGER_INCLUDES_H

#if defined(_WIN32)
 #define APIENTRY __stdcall
#else
    #define APIENTRY
#endif
#include "key_defines.h"

#include <vulkan/vulkan.h>

#include <stdint.h>

// Swaps the provided pointers
#define _ENGINE_SWAP(type, x, y) \
    {                          \
        type t;                \
        t = x;                 \
        x = y;                 \
        y = t;                 \
    }

#define _ENGINE_INSERT_FIRST      0
#define _ENGINE_INSERT_LAST       1

#define GL_VERSION 0x1f02
#define GL_NONE 0
#define GL_COLOR_BUFFER_BIT 0x00004000
#define GL_UNSIGNED_BYTE 0x1401
#define GL_EXTENSIONS 0x1f03
#define GL_NUM_EXTENSIONS 0x821d
#define GL_CONTEXT_FLAGS 0x821e
#define GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT 0x00000001
#define GL_CONTEXT_FLAG_DEBUG_BIT 0x00000002
#define GL_CONTEXT_PROFILE_MASK 0x9126
#define GL_CONTEXT_COMPATIBILITY_PROFILE_BIT 0x00000002
#define GL_CONTEXT_CORE_PROFILE_BIT 0x00000001
#define GL_RESET_NOTIFICATION_STRATEGY_ARB 0x8256
#define GL_LOSE_CONTEXT_ON_RESET_ARB 0x8252
#define GL_NO_RESET_NOTIFICATION_ARB 0x8261
#define GL_CONTEXT_RELEASE_BEHAVIOR 0x82fb
#define GL_CONTEXT_RELEASE_BEHAVIOR_FLUSH 0x82fc
#define GL_CONTEXT_FLAG_NO_ERROR_BIT_KHR 0x00000008

#define ENGINE_NO_API                          0

#define ENGINE_PLATFORM_UNAVAILABLE   0x0001000E

#define ENGINE_RESIZABLE              0x00020003

#define ENGINE_CLIENT_API             0x00022001

#define ENGINE_FOCUSED                0x00020001
#define ENGINE_ICONIFIED              0x00020002
#define ENGINE_RESIZABLE              0x00020003
#define ENGINE_VISIBLE                0x00020004
#define ENGINE_DECORATED              0x00020005
#define ENGINE_AUTO_ICONIFY           0x00020006
#define ENGINE_FLOATING               0x00020007
#define ENGINE_MAXIMIZED              0x00020008
#define ENGINE_CENTER_CURSOR          0x00020009
#define ENGINE_TRANSPARENT_FRAMEBUFFER 0x0002000A
#define ENGINE_HOVERED                0x0002000B
#define ENGINE_FOCUS_ON_SHOW          0x0002000C
#define ENGINE_MOUSE_PASSTHROUGH      0x0002000D
#define ENGINE_POSITION_X             0x0002000E
#define ENGINE_POSITION_Y             0x0002000F
#define ENGINE_RED_BITS               0x00021001
#define ENGINE_GREEN_BITS             0x00021002
#define ENGINE_BLUE_BITS              0x00021003
#define ENGINE_ALPHA_BITS             0x00021004
#define ENGINE_DEPTH_BITS             0x00021005
#define ENGINE_STENCIL_BITS           0x00021006
#define ENGINE_ACCUM_RED_BITS         0x00021007
#define ENGINE_ACCUM_GREEN_BITS       0x00021008
#define ENGINE_ACCUM_BLUE_BITS        0x00021009
#define ENGINE_ACCUM_ALPHA_BITS       0x0002100A
#define ENGINE_AUX_BUFFERS            0x0002100B
#define ENGINE_STEREO                 0x0002100C
#define ENGINE_SAMPLES                0x0002100D
#define ENGINE_SRGB_CAPABLE           0x0002100E
#define ENGINE_REFRESH_RATE           0x0002100F
#define ENGINE_DOUBLEBUFFER           0x00021010
#define ENGINE_CLIENT_API             0x00022001
#define ENGINE_CONTEXT_VERSION_MAJOR  0x00022002
#define ENGINE_CONTEXT_VERSION_MINOR  0x00022003
#define ENGINE_CONTEXT_REVISION       0x00022004
#define ENGINE_CONTEXT_ROBUSTNESS     0x00022005
#define ENGINE_OPENGL_FORWARD_COMPAT  0x00022006
#define ENGINE_CONTEXT_DEBUG          0x00022007
#define ENGINE_OPENGL_DEBUG_CONTEXT   ENGINE_CONTEXT_DEBUG
#define ENGINE_OPENGL_PROFILE         0x00022008
#define ENGINE_CONTEXT_RELEASE_BEHAVIOR 0x00022009
#define ENGINE_CONTEXT_NO_ERROR       0x0002200A
#define ENGINE_CONTEXT_CREATION_API   0x0002200B
#define ENGINE_SCALE_TO_MONITOR       0x0002200C
#define ENGINE_COCOA_RETINA_FRAMEBUFFER 0x00023001
#define ENGINE_COCOA_FRAME_NAME         0x00023002
#define ENGINE_COCOA_GRAPHICS_SWITCHING 0x00023003
#define ENGINE_X11_CLASS_NAME         0x00024001
#define ENGINE_X11_INSTANCE_NAME      0x00024002
#define ENGINE_WIN32_KEYBOARD_MENU    0x00025001

#define ENGINE_NO_API                          0
#define ENGINE_OPENGL_API             0x00030001
#define ENGINE_OPENGL_ES_API          0x00030002

#define ENGINE_NATIVE_CONTEXT_API     0x00036001
#define ENGINE_EGL_CONTEXT_API        0x00036002
#define ENGINE_OSMESA_CONTEXT_API     0x00036003

#define ENGINE_NO_ROBUSTNESS                   0
#define ENGINE_NO_RESET_NOTIFICATION  0x00031001
#define ENGINE_LOSE_CONTEXT_ON_RESET  0x00031002

#define ENGINE_OPENGL_ANY_PROFILE              0
#define ENGINE_OPENGL_CORE_PROFILE    0x00032001
#define ENGINE_OPENGL_COMPAT_PROFILE  0x00032002

#define ENGINE_ANY_RELEASE_BEHAVIOR            0
#define ENGINE_RELEASE_BEHAVIOR_FLUSH 0x00035001
#define ENGINE_RELEASE_BEHAVIOR_NONE  0x00035002

#define ENGINE_CONNECTED              0x00040001
#define ENGINE_DISCONNECTED           0x00040002

#define ENGINE_ANY_PLATFORM           0x00060000
#define ENGINE_PLATFORM_WIN32         0x00060001
#define ENGINE_PLATFORM_COCOA         0x00060002
#define ENGINE_PLATFORM_WAYLAND       0x00060003
#define ENGINE_PLATFORM_X11           0x00060004
#define ENGINE_PLATFORM_NULL          0x00060005

typedef void* EGLConfig;
typedef void* EGLContext;
typedef void* EGLDisplay;
typedef void* EGLSurface;

typedef void* OSMesaContext;
typedef void (*OSMESAproc)(void);

typedef void (APIENTRY * PFNGLCLEARPROC)(uint32_t);
typedef const uint8_t* (APIENTRY * PFNGLGETSTRINGPROC)(uint32_t);
typedef void (APIENTRY * PFNGLGETINTEGERVPROC)(uint32_t,int32_t*);
typedef const uint8_t* (APIENTRY * PFNGLGETSTRINGIPROC)(uint32_t,uint32_t);

typedef struct wManagerWindow wManagerWindow;

typedef struct wManagerImage
{
    /*! The width, in pixels, of this image.
     */
    int width;
    /*! The height, in pixels, of this image.
     */
    int height;
    /*! The pixel data of this image, arranged left-to-right, top-to-bottom.
     */
    unsigned char* pixels;
} wManagerImage;

// Context structure
//
typedef struct _wManagercontext
{
    int                 client;
    int                 source;
    int                 major, minor, revision;
    int32_t            forward, debug, noerror;
    int                 profile;
    int                 robustness;
    int                 release;

    PFNGLGETSTRINGIPROC  GetStringi;
    PFNGLGETINTEGERVPROC GetIntegerv;
    PFNGLGETSTRINGPROC   GetString;

    void (*makeCurrent)(wManagerWindow*);
    void (*swapBuffers)(wManagerWindow*);
    void (*swapInterval)(int);
    int (*extensionSupported)(const char*);
    void* (*getProcAddress)(const char*);
    void (*destroy)(wManagerWindow*);

    struct {
        EGLConfig       config;
        EGLContext      handle;
        EGLSurface      surface;
        void*           client;
    } egl;

    struct {
        OSMesaContext   handle;
        int             width;
        int             height;
        void*           buffer;
    } osmesa;

} _wManagercontext;

// Initialization configuration
//
// Parameters relating to the initialization of the library
//
typedef struct _wManagerinitconfig
{
    int32_t      hatButtons;
    int           angleType;
    int           platformID;
    PFN_vkGetInstanceProcAddr vulkanLoader;
    struct {
        int32_t  menubar;
        int32_t  chdir;
    } ns;
    struct {
        int32_t  xcbVulkanSurface;
    } x11;
    struct {
        int       libdecorMode;
    } wl;
} _wManagerinitconfig;

// Window configuration
//
// Parameters relating to the creation of the window but not directly related
// to the framebuffer.  This is used to pass window creation parameters from
// shared code to the platform API.
//
typedef struct _wManagerwndconfig
{
    int           xpos;
    int           ypos;
    int           width;
    int           height;
    const char*   title;
    int32_t      resizable;
    int32_t      visible;
    int32_t      decorated;
    int32_t      focused;
    int32_t      autoIconify;
    int32_t      floating;
    int32_t      maximized;
    int32_t      centerCursor;
    int32_t      focusOnShow;
    int32_t      mousePassthrough;
    int32_t      scaleToMonitor;
    struct {
        int32_t  retina;
        char      frameName[256];
    } ns;
    struct {
        char      className[256];
        char      instanceName[256];
    } x11;
    struct {
        int32_t  keymenu;
        int32_t  showDefault;
    } win32;
    struct {
        char      appId[256];
    } wl;
} _wManagerwndconfig;

// Framebuffer configuration
//
// This describes buffers and their sizes.  It also contains
// a platform-specific ID used to map back to the backend API object.
//
// It is used to pass framebuffer parameters from shared code to the platform
// API and also to enumerate and select available framebuffer configs.
//
typedef struct _wManagerfbconfig
{
    int         redBits;
    int         greenBits;
    int         blueBits;
    int         alphaBits;
    int         depthBits;
    int         stencilBits;
    int         accumRedBits;
    int         accumGreenBits;
    int         accumBlueBits;
    int         accumAlphaBits;
    int         auxBuffers;
    int32_t    stereo;
    int         samples;
    int32_t    sRGB;
    int32_t    doublebuffer;
    int32_t    transparent;
    uintptr_t   handle;
} _wManagerfbconfig;

typedef struct wManagerVidmode
{
    /*! The width, in screen coordinates, of the video mode.
     */
    int width;
    /*! The height, in screen coordinates, of the video mode.
     */
    int height;
    /*! The bit depth of the red channel of the video mode.
     */
    int redBits;
    /*! The bit depth of the green channel of the video mode.
     */
    int greenBits;
    /*! The bit depth of the blue channel of the video mode.
     */
    int blueBits;
    /*! The refresh rate, in Hz, of the video mode.
     */
    int refreshRate;
} wManagerVidmode;

typedef struct wManagerGammaramp
{
    /*! An array of value describing the response of the red channel.
     */
    unsigned short* red;
    /*! An array of value describing the response of the green channel.
     */
    unsigned short* green;
    /*! An array of value describing the response of the blue channel.
     */
    unsigned short* blue;
    /*! The number of elements in each array.
     */
    unsigned int size;
} wManagerGammaramp;

// Monitor structure
//
typedef struct _wManagerMonitor
{
    char            name[128];
    void*           userPointer;

    // Physical dimensions in millimeters.
    int             widthMM, heightMM;

    // The window whose video mode is current on this monitor
    wManagerWindow*    window;

    wManagerVidmode*    modes;
    int             modeCount;
    wManagerVidmode     currentMode;

    wManagerGammaramp   originalRamp;
    wManagerGammaramp   currentRamp;

    void *MonitorData;
} _wManagerMonitor;

// Platform API structure
//
typedef struct _wManagerPlatform
{
    int platformID;
    // init
    uint32_t (*init)(void);
    void (*terminate)(void);
    // input
    void (*getCursorPos)(wManagerWindow*,double*,double*);
    void (*setCursorPos)(wManagerWindow*,double,double);
    void (*setCursorMode)(wManagerWindow*,int);
    void (*setRawMouseMotion)(wManagerWindow*,uint32_t);
    uint32_t (*rawMouseMotionSupported)(void);
    //uint32_t (*createCursor)(_GLFWcursor*,const GLFWimage*,int,int);
    //uint32_t (*createStandardCursor)(_GLFWcursor*,int);
    //void (*destroyCursor)(_GLFWcursor*);
    //void (*setCursor)(wManagerWindow*,_GLFWcursor*);
    const char* (*getScancodeName)(int);
    int (*getKeyScancode)(int);
    void (*setClipboardString)(const char*);
    const char* (*getClipboardString)(void);
    //uint32_t (*initJoysticks)(void);
    //void (*terminateJoysticks)(void);
    //uint32_t (*pollJoystick)(_GLFWjoystick*,int);
    //const char* (*getMappingName)(void);
    //void (*updateGamepadGUID)(char*);
    // monitor
    //void (*freeMonitor)(_wManagerMonitor*);
    //void (*getMonitorPos)(_wManagerMonitor*,int*,int*);
    //void (*getMonitorContentScale)(_wManagerMonitor*,float*,float*);
    //void (*getMonitorWorkarea)(_wManagerMonitor*,int*,int*,int*,int*);
    //wManagerVidmode* (*getVideoModes)(_wManagerMonitor*,int*);
    //void (*getVideoMode)(_wManagerMonitor*,wManagerVidmode*);
    //uint32_t (*getGammaRamp)(_wManagerMonitor*,wManagerGammaramp*);
    //void (*setGammaRamp)(_wManagerMonitor*,const wManagerGammaramp*);
    // window
    uint32_t (*createWindow)(wManagerWindow*,const _wManagerwndconfig*,const _wManagerfbconfig*);
    void (*destroyWindow)(wManagerWindow*);
    void (*setWindowTitle)(wManagerWindow*,const char*);
    //void (*setWindowIcon)(wManagerWindow*,int,const GLFWimage*);
    void (*getWindowPos)(wManagerWindow*,int*,int*);
    void (*setWindowPos)(wManagerWindow*,int,int);
    void (*getWindowSize)(wManagerWindow*,int*,int*);
    void (*setWindowSize)(wManagerWindow*,int,int);
    void (*setWindowSizeLimits)(wManagerWindow*,int,int,int,int);
    void (*setWindowAspectRatio)(wManagerWindow*,int,int);
    void (*getFramebufferSize)(wManagerWindow*,int*,int*);
    void (*getWindowFrameSize)(wManagerWindow*,int*,int*,int*,int*);
    void (*getWindowContentScale)(wManagerWindow*,float*,float*);
    void (*iconifyWindow)(wManagerWindow*);
    void (*restoreWindow)(wManagerWindow*);
    void (*maximizeWindow)(wManagerWindow*);
    void (*showWindow)(wManagerWindow*);
    void (*hideWindow)(wManagerWindow*);
    void (*requestWindowAttention)(wManagerWindow*);
    void (*focusWindow)(wManagerWindow*);
    //void (*setWindowMonitor)(wManagerWindow*,_wManagerMonitor*,int,int,int,int,int);
    uint32_t (*windowFocused)(wManagerWindow*);
    uint32_t (*windowIconified)(wManagerWindow*);
    uint32_t (*windowVisible)(wManagerWindow*);
    uint32_t (*windowMaximized)(wManagerWindow*);
    uint32_t (*windowHovered)(wManagerWindow*);
    uint32_t (*framebufferTransparent)(wManagerWindow*);
    float (*getWindowOpacity)(wManagerWindow*);
    void (*setWindowResizable)(wManagerWindow*,uint32_t);
    void (*setWindowDecorated)(wManagerWindow*,uint32_t);
    void (*setWindowFloating)(wManagerWindow*,uint32_t);
    void (*setWindowOpacity)(wManagerWindow*,float);
    void (*setWindowMousePassthrough)(wManagerWindow*,uint32_t);
    void (*pollEvents)(void);
    void (*waitEvents)(void);
    void (*waitEventsTimeout)(double);
    void (*postEmptyEvent)(void);
    // EGL
    /*EGLenum (*getEGLPlatform)(EGLint**);
    EGLNativeDisplayType (*getEGLNativeDisplay)(void);
    EGLNativeWindowType (*getEGLNativeWindow)(wManagerWindow*);*/
    // vulkan
    void (*getRequiredInstanceExtensions)(char**);
    uint32_t (*getPhysicalDevicePresentationSupport)(VkInstance,VkPhysicalDevice,uint32_t);
    VkResult (*createWindowSurface)(VkInstance,wManagerWindow*,const VkAllocationCallbacks*,VkSurfaceKHR*);
} _wManagerPlatform;

#endif // MANAGER_INCLUDES_H
