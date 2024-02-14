#ifndef MANAGER_INCLUDES_H
#define MANAGER_INCLUDES_H

#if defined(_WIN32)
 #define APIENTRY __stdcall
#else
    #define APIENTRY
#endif

#include <vulkan/vulkan.h>

#include <stdint.h>

#define ENGINE_MOD_SHIFT           0x0001
#define ENGINE_MOD_CONTROL         0x0002
#define ENGINE_MOD_ALT             0x0004
#define ENGINE_MOD_SUPER           0x0008
#define ENGINE_MOD_CAPS_LOCK       0x0010
#define ENGINE_MOD_NUM_LOCK        0x0020

#define ENGINE_MOUSE_BUTTON_1         0
#define ENGINE_MOUSE_BUTTON_2         1
#define ENGINE_MOUSE_BUTTON_3         2
#define ENGINE_MOUSE_BUTTON_4         3
#define ENGINE_MOUSE_BUTTON_5         4
#define ENGINE_MOUSE_BUTTON_6         5
#define ENGINE_MOUSE_BUTTON_7         6
#define ENGINE_MOUSE_BUTTON_8         7
#define ENGINE_MOUSE_BUTTON_LAST      ENGINE_MOUSE_BUTTON_8
#define ENGINE_MOUSE_BUTTON_LEFT      ENGINE_MOUSE_BUTTON_1
#define ENGINE_MOUSE_BUTTON_RIGHT     ENGINE_MOUSE_BUTTON_2
#define ENGINE_MOUSE_BUTTON_MIDDLE    ENGINE_MOUSE_BUTTON_3

#define ENGINE_JOYSTICK_1             0
#define ENGINE_JOYSTICK_2             1
#define ENGINE_JOYSTICK_3             2
#define ENGINE_JOYSTICK_4             3
#define ENGINE_JOYSTICK_5             4
#define ENGINE_JOYSTICK_6             5
#define ENGINE_JOYSTICK_7             6
#define ENGINE_JOYSTICK_8             7
#define ENGINE_JOYSTICK_9             8
#define ENGINE_JOYSTICK_10            9
#define ENGINE_JOYSTICK_11            10
#define ENGINE_JOYSTICK_12            11
#define ENGINE_JOYSTICK_13            12
#define ENGINE_JOYSTICK_14            13
#define ENGINE_JOYSTICK_15            14
#define ENGINE_JOYSTICK_16            15
#define ENGINE_JOYSTICK_LAST          ENGINE_JOYSTICK_16

#define ENGINE_RELEASE                0
#define ENGINE_PRESS                  1
#define ENGINE_REPEAT                 2

#define ENGINE_DONT_CARE              -1

#define ENGINE_KEY_UNKNOWN            -1

/* Printable keys */
#define ENGINE_KEY_SPACE              32
#define ENGINE_KEY_APOSTROPHE         39  /* ' */
#define ENGINE_KEY_COMMA              44  /* , */
#define ENGINE_KEY_MINUS              45  /* - */
#define ENGINE_KEY_PERIOD             46  /* . */
#define ENGINE_KEY_SLASH              47  /* / */
#define ENGINE_KEY_0                  48
#define ENGINE_KEY_1                  49
#define ENGINE_KEY_2                  50
#define ENGINE_KEY_3                  51
#define ENGINE_KEY_4                  52
#define ENGINE_KEY_5                  53
#define ENGINE_KEY_6                  54
#define ENGINE_KEY_7                  55
#define ENGINE_KEY_8                  56
#define ENGINE_KEY_9                  57
#define ENGINE_KEY_SEMICOLON          59  /* ; */
#define ENGINE_KEY_EQUAL              61  /* = */
#define ENGINE_KEY_A                  65
#define ENGINE_KEY_B                  66
#define ENGINE_KEY_C                  67
#define ENGINE_KEY_D                  68
#define ENGINE_KEY_E                  69
#define ENGINE_KEY_F                  70
#define ENGINE_KEY_G                  71
#define ENGINE_KEY_H                  72
#define ENGINE_KEY_I                  73
#define ENGINE_KEY_J                  74
#define ENGINE_KEY_K                  75
#define ENGINE_KEY_L                  76
#define ENGINE_KEY_M                  77
#define ENGINE_KEY_N                  78
#define ENGINE_KEY_O                  79
#define ENGINE_KEY_P                  80
#define ENGINE_KEY_Q                  81
#define ENGINE_KEY_R                  82
#define ENGINE_KEY_S                  83
#define ENGINE_KEY_T                  84
#define ENGINE_KEY_U                  85
#define ENGINE_KEY_V                  86
#define ENGINE_KEY_W                  87
#define ENGINE_KEY_X                  88
#define ENGINE_KEY_Y                  89
#define ENGINE_KEY_Z                  90
#define ENGINE_KEY_LEFT_BRACKET       91  /* [ */
#define ENGINE_KEY_BACKSLASH          92  /* \ */
#define ENGINE_KEY_RIGHT_BRACKET      93  /* ] */
#define ENGINE_KEY_GRAVE_ACCENT       96  /* ` */
#define ENGINE_KEY_WORLD_1            161 /* non-US #1 */
#define ENGINE_KEY_WORLD_2            162 /* non-US #2 */

/* Function keys */
#define ENGINE_KEY_ESCAPE             256
#define ENGINE_KEY_ENTER              257
#define ENGINE_KEY_TAB                258
#define ENGINE_KEY_BACKSPACE          259
#define ENGINE_KEY_INSERT             260
#define ENGINE_KEY_DELETE             261
#define ENGINE_KEY_RIGHT              262
#define ENGINE_KEY_LEFT               263
#define ENGINE_KEY_DOWN               264
#define ENGINE_KEY_UP                 265
#define ENGINE_KEY_PAGE_UP            266
#define ENGINE_KEY_PAGE_DOWN          267
#define ENGINE_KEY_HOME               268
#define ENGINE_KEY_END                269
#define ENGINE_KEY_CAPS_LOCK          280
#define ENGINE_KEY_SCROLL_LOCK        281
#define ENGINE_KEY_NUM_LOCK           282
#define ENGINE_KEY_PRINT_SCREEN       283
#define ENGINE_KEY_PAUSE              284
#define ENGINE_KEY_F1                 290
#define ENGINE_KEY_F2                 291
#define ENGINE_KEY_F3                 292
#define ENGINE_KEY_F4                 293
#define ENGINE_KEY_F5                 294
#define ENGINE_KEY_F6                 295
#define ENGINE_KEY_F7                 296
#define ENGINE_KEY_F8                 297
#define ENGINE_KEY_F9                 298
#define ENGINE_KEY_F10                299
#define ENGINE_KEY_F11                300
#define ENGINE_KEY_F12                301
#define ENGINE_KEY_F13                302
#define ENGINE_KEY_F14                303
#define ENGINE_KEY_F15                304
#define ENGINE_KEY_F16                305
#define ENGINE_KEY_F17                306
#define ENGINE_KEY_F18                307
#define ENGINE_KEY_F19                308
#define ENGINE_KEY_F20                309
#define ENGINE_KEY_F21                310
#define ENGINE_KEY_F22                311
#define ENGINE_KEY_F23                312
#define ENGINE_KEY_F24                313
#define ENGINE_KEY_F25                314
#define ENGINE_KEY_KP_0               320
#define ENGINE_KEY_KP_1               321
#define ENGINE_KEY_KP_2               322
#define ENGINE_KEY_KP_3               323
#define ENGINE_KEY_KP_4               324
#define ENGINE_KEY_KP_5               325
#define ENGINE_KEY_KP_6               326
#define ENGINE_KEY_KP_7               327
#define ENGINE_KEY_KP_8               328
#define ENGINE_KEY_KP_9               329
#define ENGINE_KEY_KP_DECIMAL         330
#define ENGINE_KEY_KP_DIVIDE          331
#define ENGINE_KEY_KP_MULTIPLY        332
#define ENGINE_KEY_KP_SUBTRACT        333
#define ENGINE_KEY_KP_ADD             334
#define ENGINE_KEY_KP_ENTER           335
#define ENGINE_KEY_KP_EQUAL           336
#define ENGINE_KEY_LEFT_SHIFT         340
#define ENGINE_KEY_LEFT_CONTROL       341
#define ENGINE_KEY_LEFT_ALT           342
#define ENGINE_KEY_LEFT_SUPER         343
#define ENGINE_KEY_RIGHT_SHIFT        344
#define ENGINE_KEY_RIGHT_CONTROL      345
#define ENGINE_KEY_RIGHT_ALT          346
#define ENGINE_KEY_RIGHT_SUPER        347
#define ENGINE_KEY_MENU               348

#define ENGINE_KEY_LAST               ENGINE_KEY_MENU

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

#define ENGINE_CURSOR                 0x00033001
#define ENGINE_STICKY_KEYS            0x00033002
#define ENGINE_STICKY_MOUSE_BUTTONS   0x00033003
#define ENGINE_LOCK_KEY_MODS          0x00033004
#define ENGINE_RAW_MOUSE_MOTION       0x00033005

#define ENGINE_CURSOR_NORMAL          0x00034001
#define ENGINE_CURSOR_HIDDEN          0x00034002
#define ENGINE_CURSOR_DISABLED        0x00034003
#define ENGINE_CURSOR_CAPTURED        0x00034004

#define ENGINE_ANY_POSITION           0x80000000

typedef struct wManagerWindow wManagerWindow;

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
    } win32;
    struct {
        char      appId[256];
    } wl;
} _wManagerwndconfig;

// Context configuration
//
// Parameters relating to the creation of the context but not directly related
// to the framebuffer.  This is used to pass context creation parameters from
// shared code to the platform API.
//
typedef struct _wManagerctxconfig
{
    int           client;
    int           source;
    int           major;
    int           minor;
    int32_t      forward;
    int32_t      debug;
    int32_t      noerror;
    int           profile;
    int           robustness;
    int           release;
    //_GLFWwindow*  share;
    struct {
        int32_t  offline;
    } nsgl;
} _wManagerctxconfig;

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
    /*void (*freeMonitor)(_GLFWmonitor*);
    void (*getMonitorPos)(_GLFWmonitor*,int*,int*);
    void (*getMonitorContentScale)(_GLFWmonitor*,float*,float*);
    void (*getMonitorWorkarea)(_GLFWmonitor*,int*,int*,int*,int*);
    GLFWvidmode* (*getVideoModes)(_GLFWmonitor*,int*);
    void (*getVideoMode)(_GLFWmonitor*,GLFWvidmode*);
    uint32_t (*getGammaRamp)(_GLFWmonitor*,GLFWgammaramp*);
    void (*setGammaRamp)(_GLFWmonitor*,const GLFWgammaramp*);*/
    // window
    uint32_t (*createWindow)(wManagerWindow*,const _wManagerwndconfig*,const _wManagerctxconfig*,const _wManagerfbconfig*);
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
    //void (*setWindowMonitor)(wManagerWindow*,_GLFWmonitor*,int,int,int,int,int);
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
