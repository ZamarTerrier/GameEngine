#include "wManager/window_manager.h"
#include "wManager/win_defines.h"

#ifdef _WIN_

extern wManagerWindow _wMWindow;

// Create key code translation tables
//
void createKeyTables(void *wData)
{
    int scancode;

    wManagerWin* windowData = wData;

    memset(windowData->keycodes, -1, sizeof(windowData->keycodes));
    memset(windowData->scancodes, -1, sizeof(windowData->scancodes));

    windowData->keycodes[0x00B] = ENGINE_KEY_0;
    windowData->keycodes[0x002] = ENGINE_KEY_1;
    windowData->keycodes[0x003] = ENGINE_KEY_2;
    windowData->keycodes[0x004] = ENGINE_KEY_3;
    windowData->keycodes[0x005] = ENGINE_KEY_4;
    windowData->keycodes[0x006] = ENGINE_KEY_5;
    windowData->keycodes[0x007] = ENGINE_KEY_6;
    windowData->keycodes[0x008] = ENGINE_KEY_7;
    windowData->keycodes[0x009] = ENGINE_KEY_8;
    windowData->keycodes[0x00A] = ENGINE_KEY_9;
    windowData->keycodes[0x01E] = ENGINE_KEY_A;
    windowData->keycodes[0x030] = ENGINE_KEY_B;
    windowData->keycodes[0x02E] = ENGINE_KEY_C;
    windowData->keycodes[0x020] = ENGINE_KEY_D;
    windowData->keycodes[0x012] = ENGINE_KEY_E;
    windowData->keycodes[0x021] = ENGINE_KEY_F;
    windowData->keycodes[0x022] = ENGINE_KEY_G;
    windowData->keycodes[0x023] = ENGINE_KEY_H;
    windowData->keycodes[0x017] = ENGINE_KEY_I;
    windowData->keycodes[0x024] = ENGINE_KEY_J;
    windowData->keycodes[0x025] = ENGINE_KEY_K;
    windowData->keycodes[0x026] = ENGINE_KEY_L;
    windowData->keycodes[0x032] = ENGINE_KEY_M;
    windowData->keycodes[0x031] = ENGINE_KEY_N;
    windowData->keycodes[0x018] = ENGINE_KEY_O;
    windowData->keycodes[0x019] = ENGINE_KEY_P;
    windowData->keycodes[0x010] = ENGINE_KEY_Q;
    windowData->keycodes[0x013] = ENGINE_KEY_R;
    windowData->keycodes[0x01F] = ENGINE_KEY_S;
    windowData->keycodes[0x014] = ENGINE_KEY_T;
    windowData->keycodes[0x016] = ENGINE_KEY_U;
    windowData->keycodes[0x02F] = ENGINE_KEY_V;
    windowData->keycodes[0x011] = ENGINE_KEY_W;
    windowData->keycodes[0x02D] = ENGINE_KEY_X;
    windowData->keycodes[0x015] = ENGINE_KEY_Y;
    windowData->keycodes[0x02C] = ENGINE_KEY_Z;

    windowData->keycodes[0x028] = ENGINE_KEY_APOSTROPHE;
    windowData->keycodes[0x02B] = ENGINE_KEY_BACKSLASH;
    windowData->keycodes[0x033] = ENGINE_KEY_COMMA;
    windowData->keycodes[0x00D] = ENGINE_KEY_EQUAL;
    windowData->keycodes[0x029] = ENGINE_KEY_GRAVE_ACCENT;
    windowData->keycodes[0x01A] = ENGINE_KEY_LEFT_BRACKET;
    windowData->keycodes[0x00C] = ENGINE_KEY_MINUS;
    windowData->keycodes[0x034] = ENGINE_KEY_PERIOD;
    windowData->keycodes[0x01B] = ENGINE_KEY_RIGHT_BRACKET;
    windowData->keycodes[0x027] = ENGINE_KEY_SEMICOLON;
    windowData->keycodes[0x035] = ENGINE_KEY_SLASH;
    windowData->keycodes[0x056] = ENGINE_KEY_WORLD_2;

    windowData->keycodes[0x00E] = ENGINE_KEY_BACKSPACE;
    windowData->keycodes[0x153] = ENGINE_KEY_DELETE;
    windowData->keycodes[0x14F] = ENGINE_KEY_END;
    windowData->keycodes[0x01C] = ENGINE_KEY_ENTER;
    windowData->keycodes[0x001] = ENGINE_KEY_ESCAPE;
    windowData->keycodes[0x147] = ENGINE_KEY_HOME;
    windowData->keycodes[0x152] = ENGINE_KEY_INSERT;
    windowData->keycodes[0x15D] = ENGINE_KEY_MENU;
    windowData->keycodes[0x151] = ENGINE_KEY_PAGE_DOWN;
    windowData->keycodes[0x149] = ENGINE_KEY_PAGE_UP;
    windowData->keycodes[0x045] = ENGINE_KEY_PAUSE;
    windowData->keycodes[0x039] = ENGINE_KEY_SPACE;
    windowData->keycodes[0x00F] = ENGINE_KEY_TAB;
    windowData->keycodes[0x03A] = ENGINE_KEY_CAPS_LOCK;
    windowData->keycodes[0x145] = ENGINE_KEY_NUM_LOCK;
    windowData->keycodes[0x046] = ENGINE_KEY_SCROLL_LOCK;
    windowData->keycodes[0x03B] = ENGINE_KEY_F1;
    windowData->keycodes[0x03C] = ENGINE_KEY_F2;
    windowData->keycodes[0x03D] = ENGINE_KEY_F3;
    windowData->keycodes[0x03E] = ENGINE_KEY_F4;
    windowData->keycodes[0x03F] = ENGINE_KEY_F5;
    windowData->keycodes[0x040] = ENGINE_KEY_F6;
    windowData->keycodes[0x041] = ENGINE_KEY_F7;
    windowData->keycodes[0x042] = ENGINE_KEY_F8;
    windowData->keycodes[0x043] = ENGINE_KEY_F9;
    windowData->keycodes[0x044] = ENGINE_KEY_F10;
    windowData->keycodes[0x057] = ENGINE_KEY_F11;
    windowData->keycodes[0x058] = ENGINE_KEY_F12;
    windowData->keycodes[0x064] = ENGINE_KEY_F13;
    windowData->keycodes[0x065] = ENGINE_KEY_F14;
    windowData->keycodes[0x066] = ENGINE_KEY_F15;
    windowData->keycodes[0x067] = ENGINE_KEY_F16;
    windowData->keycodes[0x068] = ENGINE_KEY_F17;
    windowData->keycodes[0x069] = ENGINE_KEY_F18;
    windowData->keycodes[0x06A] = ENGINE_KEY_F19;
    windowData->keycodes[0x06B] = ENGINE_KEY_F20;
    windowData->keycodes[0x06C] = ENGINE_KEY_F21;
    windowData->keycodes[0x06D] = ENGINE_KEY_F22;
    windowData->keycodes[0x06E] = ENGINE_KEY_F23;
    windowData->keycodes[0x076] = ENGINE_KEY_F24;
    windowData->keycodes[0x038] = ENGINE_KEY_LEFT_ALT;
    windowData->keycodes[0x01D] = ENGINE_KEY_LEFT_CONTROL;
    windowData->keycodes[0x02A] = ENGINE_KEY_LEFT_SHIFT;
    windowData->keycodes[0x15B] = ENGINE_KEY_LEFT_SUPER;
    windowData->keycodes[0x137] = ENGINE_KEY_PRINT_SCREEN;
    windowData->keycodes[0x138] = ENGINE_KEY_RIGHT_ALT;
    windowData->keycodes[0x11D] = ENGINE_KEY_RIGHT_CONTROL;
    windowData->keycodes[0x036] = ENGINE_KEY_RIGHT_SHIFT;
    windowData->keycodes[0x15C] = ENGINE_KEY_RIGHT_SUPER;
    windowData->keycodes[0x150] = ENGINE_KEY_DOWN;
    windowData->keycodes[0x14B] = ENGINE_KEY_LEFT;
    windowData->keycodes[0x14D] = ENGINE_KEY_RIGHT;
    windowData->keycodes[0x148] = ENGINE_KEY_UP;

    windowData->keycodes[0x052] = ENGINE_KEY_KP_0;
    windowData->keycodes[0x04F] = ENGINE_KEY_KP_1;
    windowData->keycodes[0x050] = ENGINE_KEY_KP_2;
    windowData->keycodes[0x051] = ENGINE_KEY_KP_3;
    windowData->keycodes[0x04B] = ENGINE_KEY_KP_4;
    windowData->keycodes[0x04C] = ENGINE_KEY_KP_5;
    windowData->keycodes[0x04D] = ENGINE_KEY_KP_6;
    windowData->keycodes[0x047] = ENGINE_KEY_KP_7;
    windowData->keycodes[0x048] = ENGINE_KEY_KP_8;
    windowData->keycodes[0x049] = ENGINE_KEY_KP_9;
    windowData->keycodes[0x04E] = ENGINE_KEY_KP_ADD;
    windowData->keycodes[0x053] = ENGINE_KEY_KP_DECIMAL;
    windowData->keycodes[0x135] = ENGINE_KEY_KP_DIVIDE;
    windowData->keycodes[0x11C] = ENGINE_KEY_KP_ENTER;
    windowData->keycodes[0x059] = ENGINE_KEY_KP_EQUAL;
    windowData->keycodes[0x037] = ENGINE_KEY_KP_MULTIPLY;
    windowData->keycodes[0x04A] = ENGINE_KEY_KP_SUBTRACT;

    for (scancode = 0;  scancode < 512;  scancode++)
    {
        if (windowData->keycodes[scancode] > 0)
            windowData->scancodes[windowData->keycodes[scancode]] = scancode;
    }
}

void _wManagerUpdateKeyNamesWin32(void)
{
    int key;
    BYTE state[256] = {0};

    memset(((wManagerWin* )_wMWindow.WindowData)->keynames, 0, sizeof(((wManagerWin* )_wMWindow.WindowData)->keynames));

    for (key = ENGINE_KEY_SPACE;  key <= ENGINE_KEY_LAST;  key++)
    {
        UINT vk;
        int scancode, length;
        WCHAR chars[16];

        scancode = ((wManagerWin* )_wMWindow.WindowData)->scancodes[key];
        if (scancode == -1)
            continue;

        if (key >= ENGINE_KEY_KP_0 && key <= ENGINE_KEY_KP_ADD)
        {
            const UINT vks[] = {
                VK_NUMPAD0,  VK_NUMPAD1,  VK_NUMPAD2, VK_NUMPAD3,
                VK_NUMPAD4,  VK_NUMPAD5,  VK_NUMPAD6, VK_NUMPAD7,
                VK_NUMPAD8,  VK_NUMPAD9,  VK_DECIMAL, VK_DIVIDE,
                VK_MULTIPLY, VK_SUBTRACT, VK_ADD
            };

            vk = vks[key - ENGINE_KEY_KP_0];
        }
        else
            vk = MapVirtualKeyW(scancode, MAPVK_VSC_TO_VK);

        length = ToUnicode(vk, scancode, state,
                           chars, sizeof(chars) / sizeof(WCHAR),
                           0);

        if (length == -1)
        {
            // This is a dead key, so we need a second simulated key press
            // to make it output its own character (usually a diacritic)
            length = ToUnicode(vk, scancode, state,
                               chars, sizeof(chars) / sizeof(WCHAR),
                               0);
        }

        if (length < 1)
            continue;

        WideCharToMultiByte(CP_UTF8, 0, chars, 1,
                            ((wManagerWin* )_wMWindow.WindowData)->keynames[key],
                            sizeof(((wManagerWin* )_wMWindow.WindowData)->keynames[key]),
                            NULL, NULL);
    }
}

int _wManagerInitWin32(void)
{
    /*if (!loadLibraries())
        return false;*/

    _wManagerUpdateKeyNamesWin32();

    /*if (_wManagerIsWindows10Version1703OrGreaterWin32())
        SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);
    else if (IsWindows8Point1OrGreater())
        SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
    else if (IsWindowsVistaOrGreater())
        SetProcessDPIAware();

    if (!createHelperWindow())
        return GLFW_FALSE;*/

    //Some hacks are needed to support Remote Desktop...
    /*initRemoteSession();
    if (_wMWindow.isRemoteSession && _wMWindow.blankCursor == NULL )
    {
        _wManagerInputErrorWin32(GLFW_PLATFORM_ERROR,
                             "Win32: Failed to create blank cursor for remote session.");
        return GLFW_FALSE;
    }

    _wManagerPollMonitorsWin32();*/
    return true;
}

void _wManagerTerminateWin32(void)
{
    /*if (_wMWindow.blankCursor)
        DestroyCursor(_wMWindow.blankCursor);

    if (_wMWindow.deviceNotificationHandle)
        UnregisterDeviceNotification(_wMWindow.deviceNotificationHandle);

    if (_wMWindow.helperWindowHandle)
        DestroyWindow(_wMWindow.helperWindowHandle);
    if (_wMWindow.helperWindowClass)
        UnregisterClassW(MAKEINTATOM(_wMWindow.helperWindowClass), _wMWindow.instance);
    if (_wMWindow.mainWindowClass)
        UnregisterClassW(MAKEINTATOM(_wMWindow.mainWindowClass), _wMWindow.instance);*/

    free(((wManagerWin *)_wMWindow.WindowData)->clipboardString);
    free(((wManagerWin *)_wMWindow.WindowData)->rawInput);

    /*_wManagerTerminateWGL();
    _wManagerTerminateEGL();
    _wManagerTerminateOSMesa();*/

    //freeLibraries();
}

extern void _wManagerGetCursorPosWin32(wManagerWindow *window, double *xpos, double *ypos);
extern void _wManagerSetCursorPosWin32(wManagerWindow *window, double xpos, double ypos);
extern void _wManagerSetCursorModeWin32(wManagerWindow* window, int mode);
extern void _wManagerSetRawMouseMotionWin32(wManagerWindow *window, int32_t enabled);
extern int32_t _wManagerRawMouseMotionSupportedWin32(void);
extern const char* _wManagerGetScancodeNameWin32(int scancode);
extern int _wManagerGetKeyScancodeWin32(int key);
extern void _wManagerSetClipboardStringWin32(const char* string);
extern void _wManagerSetWindowMonitorWin32(wManagerWindow* window,
                                           _wManagerMonitor* monitor,
                                           int xpos, int ypos,
                                           int width, int height,
                                           int refreshRate);
extern const char* _wManagerGetClipboardStringWin32(void);
extern int _wManagerCreateWindowWin32(wManagerWindow *window, const _wManagerwndconfig* wndconfig, const _wManagerfbconfig* fbconfig);
extern void _wManagerDestroyWindowWin32(wManagerWindow* window);
extern void _wManagerSetWindowTitleWin32(wManagerWindow* window, const char* title);
extern void _wManagerSetWindowIconWin32(wManagerWindow* window, int count, void* images);
extern void _wManagerGetWindowPosWin32(wManagerWindow* window, int* xpos, int* ypos);
extern void _wManagerSetWindowPosWin32(wManagerWindow* window, int xpos, int ypos);
extern void _wManagerGetWindowSizeWin32(wManagerWindow *window, int* width, int* height);
extern void _wManagerSetWindowSizeWin32(wManagerWindow* window, int width, int height);
extern void _wManagerSetWindowSizeLimitsWin32(wManagerWindow* window,
                                              int minwidth, int minheight,
                                              int maxwidth, int maxheight);
extern void _wManagerSetWindowAspectRatioWin32(wManagerWindow* window, int numer, int denom);
extern void _wManagerGetFramebufferSizeWin32(wManagerWindow* window, int* width, int* height);
extern void _wManagerGetWindowFrameSizeWin32(wManagerWindow* window,
                                             int* left, int* top,
                                             int* right, int* bottom);
extern void _wManagerGetWindowContentScaleWin32(wManagerWindow* window, float* xscale, float* yscale);
extern void _wManagerIconifyWindowWin32(wManagerWindow* window);
extern void _wManagerRestoreWindowWin32(wManagerWindow* window);
extern void _wManagerMaximizeWindowWin32(wManagerWindow* window);
extern void _wManagerShowWindowWin32(wManagerWindow* window);
extern void _wManagerHideWindowWin32(wManagerWindow* window);
extern void _wManagerRequestWindowAttentionWin32(wManagerWindow* window);
extern void _wManagerFocusWindowWin32(wManagerWindow* window);
extern int32_t _wManagerWindowFocusedWin32(wManagerWindow* window);
extern int32_t _wManagerWindowIconifiedWin32(wManagerWindow* window);
extern int32_t _wManagerWindowVisibleWin32(wManagerWindow* window);
extern int32_t _wManagerWindowMaximizedWin32(wManagerWindow* window);
extern int32_t _wManagerWindowHoveredWin32(wManagerWindow* window);
extern int32_t _wManagerFramebufferTransparentWin32(wManagerWindow* window);
extern float _wManagerGetWindowOpacityWin32(wManagerWindow* window);
extern void _wManagerSetWindowResizableWin32(wManagerWindow* window, int32_t enabled);
extern void _wManagerSetWindowDecoratedWin32(wManagerWindow* window, int32_t enabled);
extern void _wManagerSetWindowFloatingWin32(wManagerWindow* window, int32_t enabled);
extern void _wManagerSetWindowOpacityWin32(wManagerWindow* window, float opacity);
extern void _wManagerSetWindowMousePassthroughWin32(wManagerWindow* window, int32_t enabled);
extern void _wManagerPollEventsWin32();
extern void _wManagerWaitEventsWin32(void);
extern void _wManagerWaitEventsTimeoutWin32(double timeout);
extern void _wManagerPostEmptyEventWin32(void);
extern void _wManagerGetRequiredInstanceExtensionsWin32(char** extensions);
extern int32_t _wManagerGetPhysicalDevicePresentationSupportWin32(VkInstance instance,
                                                                  VkPhysicalDevice device,
                                                                  uint32_t queuefamily);
extern VkResult _wManagerCreateWindowSurfaceWin32(VkInstance instance,
                                                  wManagerWindow* window,
                                                  const VkAllocationCallbacks* allocator,
                                                  VkSurfaceKHR* surface);


int32_t _wManagerConnectWin32(_wManagerPlatform* platform)
{
    const _wManagerPlatform win32 =
    {
        .platformID = 0,
        .init = _wManagerInitWin32,
        .terminate = _wManagerTerminateWin32,
        .getCursorPos = _wManagerGetCursorPosWin32,
        .setCursorPos = _wManagerSetCursorPosWin32,
        .setCursorMode = _wManagerSetCursorModeWin32,
        .setRawMouseMotion = _wManagerSetRawMouseMotionWin32,
        .rawMouseMotionSupported = _wManagerRawMouseMotionSupportedWin32,
        //.createCursor = _wManagerCreateCursorWin32,
        //.createStandardCursor = _wManagerCreateStandardCursorWin32,
        //.destroyCursor = _wManagerDestroyCursorWin32,
        //.setCursor = _wManagerSetCursorWin32,
        .getScancodeName = _wManagerGetScancodeNameWin32,
        .getKeyScancode = _wManagerGetKeyScancodeWin32,
        .setClipboardString = _wManagerSetClipboardStringWin32,
        .getClipboardString = _wManagerGetClipboardStringWin32,
        //.initJoysticks = _wManagerInitJoysticksWin32,
        //.terminateJoysticks = _wManagerTerminateJoysticksWin32,
        //.pollJoystick = _wManagerPollJoystickWin32,
        //.getMappingName = _wManagerGetMappingNameWin32,
        //.updateGamepadGUID = _wManagerUpdateGamepadGUIDWin32,
        //.freeMonitor = _wManagerFreeMonitorWin32,
        //.getMonitorPos = _wManagerGetMonitorPosWin32,
        //.getMonitorContentScale = _wManagerGetMonitorContentScaleWin32,
        //.getMonitorWorkarea = _wManagerGetMonitorWorkareaWin32,
        //.getVideoModes = _wManagerGetVideoModesWin32,
        //.getVideoMode = _wManagerGetVideoModeWin32,
        //.getGammaRamp = _wManagerGetGammaRampWin32,
        //.setGammaRamp = _wManagerSetGammaRampWin32,
        .createWindow = _wManagerCreateWindowWin32,
        .destroyWindow = _wManagerDestroyWindowWin32,
        .setWindowTitle = _wManagerSetWindowTitleWin32,
        //.setWindowIcon = _wManagerSetWindowIconWin32,
        .getWindowPos = _wManagerGetWindowPosWin32,
        .setWindowPos = _wManagerSetWindowPosWin32,
        .getWindowSize = _wManagerGetWindowSizeWin32,
        .setWindowSize = _wManagerSetWindowSizeWin32,
        .setWindowSizeLimits = _wManagerSetWindowSizeLimitsWin32,
        .setWindowAspectRatio = _wManagerSetWindowAspectRatioWin32,
        .getFramebufferSize = _wManagerGetFramebufferSizeWin32,
        .getWindowFrameSize = _wManagerGetWindowFrameSizeWin32,
        .getWindowContentScale = _wManagerGetWindowContentScaleWin32,
        .iconifyWindow = _wManagerIconifyWindowWin32,
        .restoreWindow = _wManagerRestoreWindowWin32,
        .maximizeWindow = _wManagerMaximizeWindowWin32,
        .showWindow = _wManagerShowWindowWin32,
        .hideWindow = _wManagerHideWindowWin32,
        .requestWindowAttention = _wManagerRequestWindowAttentionWin32,
        .focusWindow = _wManagerFocusWindowWin32,
        //.setWindowMonitor = _wManagerSetWindowMonitorWin32,
        .windowFocused = _wManagerWindowFocusedWin32,
        .windowIconified = _wManagerWindowIconifiedWin32,
        .windowVisible = _wManagerWindowVisibleWin32,
        .windowMaximized = _wManagerWindowMaximizedWin32,
        .windowHovered = _wManagerWindowHoveredWin32,
        .framebufferTransparent = _wManagerFramebufferTransparentWin32,
        .getWindowOpacity = _wManagerGetWindowOpacityWin32,
        .setWindowResizable = _wManagerSetWindowResizableWin32,
        .setWindowDecorated = _wManagerSetWindowDecoratedWin32,
        .setWindowFloating = _wManagerSetWindowFloatingWin32,
        .setWindowOpacity = _wManagerSetWindowOpacityWin32,
        .setWindowMousePassthrough = _wManagerSetWindowMousePassthroughWin32,
        .pollEvents = _wManagerPollEventsWin32,
        .waitEvents = _wManagerWaitEventsWin32,
        .waitEventsTimeout = _wManagerWaitEventsTimeoutWin32,
        .postEmptyEvent = _wManagerPostEmptyEventWin32,
        //.getEGLPlatform = _wManagerGetEGLPlatformWin32,
        //.getEGLNativeDisplay = _wManagerGetEGLNativeDisplayWin32,
        //.getEGLNativeWindow = _wManagerGetEGLNativeWindowWin32,
        .getRequiredInstanceExtensions = _wManagerGetRequiredInstanceExtensionsWin32,
        .getPhysicalDevicePresentationSupport = _wManagerGetPhysicalDevicePresentationSupportWin32,
        .createWindowSurface = _wManagerCreateWindowSurfaceWin32
    };

    *platform = win32;
    return true;
}

#endif
