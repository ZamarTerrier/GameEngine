#include "wManager/manager_includes.h"
#include"wManager/input_manager.h"

extern wManagerWindow _wMWindow;

#ifdef _WIN_

int _wManager_min(int a, int b)
{
    return a < b ? a : b;
}

void* _wManagerPlatformLoadModule(const char* path)
{
    return LoadLibraryA(path);
}

void _wManagerPlatformFreeModule(void* module)
{
    FreeLibrary((HMODULE) module);
}

void* _wManagerPlatformGetModuleSymbol(void* module, const char* name)
{
    return (void*) GetProcAddress((HMODULE) module, name);
}

void _wManagerPlatformInitTimer(void)
{
    QueryPerformanceFrequency((LARGE_INTEGER*) &_wMWindow.time_freq);
}

uint64_t _wManagerPlatformGetTimerFrequency(void)
{
    return _wMWindow.time_freq;
}

uint64_t _wManagerPlatformGetTimerValue(void)
{
    uint64_t value;
    QueryPerformanceCounter((LARGE_INTEGER*) &value);
    return value;
}

DWORD getWindowStyle(const wManagerWindow* window)
{
    DWORD style = WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    /*if (window->monitor)
        style |= WS_POPUP;
    else*/
    {
        style |= WS_SYSMENU | WS_MINIMIZEBOX;

        if (_wMWindow.hints.window.decorated)
        {
            style |= WS_CAPTION;

            if (_wMWindow.hints.window.resizable)
                style |= WS_MAXIMIZEBOX | WS_THICKFRAME;
        }
        else
            style |= WS_POPUP;
    }

    /*{
        style |= WS_SYSMENU | WS_MINIMIZEBOX;

        if (window->hints.window.decorated)
        {
            style |= WS_CAPTION;

            if (window->hints.window.resizable)
                style |= WS_MAXIMIZEBOX | WS_THICKFRAME;
        }
        else
            style |= WS_POPUP;
    }*/

    return style;
}

DWORD getWindowExStyle(const wManagerWindow* window)
{
    DWORD style = WS_EX_APPWINDOW;

    /*if (window->monitor || window->hints.window.floating)
        style |= WS_EX_TOPMOST;*/

    return style;
}


// Retrieves and translates modifier keys
//
int getKeyMods(void)
{
    int mods = 0;

    if (GetKeyState(VK_SHIFT) & 0x8000)
        mods |= ENGINE_MOD_SHIFT;
    if (GetKeyState(VK_CONTROL) & 0x8000)
        mods |= ENGINE_MOD_CONTROL;
    if (GetKeyState(VK_MENU) & 0x8000)
        mods |= ENGINE_MOD_ALT;
    if ((GetKeyState(VK_LWIN) | GetKeyState(VK_RWIN)) & 0x8000)
        mods |= ENGINE_MOD_SUPER;
    if (GetKeyState(VK_CAPITAL) & 1)
        mods |= ENGINE_MOD_CAPS_LOCK;
    if (GetKeyState(VK_NUMLOCK) & 1)
        mods |= ENGINE_MOD_NUM_LOCK;

    return mods;
}

// Returns a UTF-8 string version of the specified wide string
//
char* _wManagerCreateUTF8FromWideStringWin32(const WCHAR* source)
{
    char* target;
    int size;

    size = WideCharToMultiByte(CP_UTF8, 0, source, -1, NULL, 0, NULL, NULL);
    if (!size)
    {
        printf("Win32: Failed to convert string to UTF-8\n");
        return NULL;
    }

    target = calloc(size, 1);

    if (!WideCharToMultiByte(CP_UTF8, 0, source, -1, target, size, NULL, NULL))
    {
        printf("Win32: Failed to convert string to UTF-8\n");
        free(target);
        return NULL;
    }

    return target;
}

// Returns a wide string version of the specified UTF-8 string
//
WCHAR* _wManagerCreateWideStringFromUTF8Win32(const char* source)
{
    WCHAR* target;
    int count;

    count = MultiByteToWideChar(CP_UTF8, 0, source, -1, NULL, 0);
    if (!count)
    {
        printf("Win32: Failed to convert string from UTF-8\n");
        return NULL;
    }

    target = calloc(count, sizeof(WCHAR));

    if (!MultiByteToWideChar(CP_UTF8, 0, source, -1, target, count))
    {
        printf("Win32: Failed to convert string from UTF-8\n");
        free(target);
        return NULL;
    }

    return target;
}

void applyAspectRatio(wManagerWindow* window, int edge, RECT* area)
{
    RECT frame = {0};
    const float ratio = (float) window->numer / (float) window->denom;
    const DWORD style = getWindowStyle(window);
    const DWORD exStyle = getWindowExStyle(window);

    AdjustWindowRectEx(&frame, style, FALSE, exStyle);

    if (edge == WMSZ_LEFT  || edge == WMSZ_BOTTOMLEFT ||
        edge == WMSZ_RIGHT || edge == WMSZ_BOTTOMRIGHT)
    {
        area->bottom = area->top + (frame.bottom - frame.top) +
            (int) (((area->right - area->left) - (frame.right - frame.left)) / ratio);
    }
    else if (edge == WMSZ_TOPLEFT || edge == WMSZ_TOPRIGHT)
    {
        area->top = area->bottom - (frame.bottom - frame.top) -
            (int) (((area->right - area->left) - (frame.right - frame.left)) / ratio);
    }
    else if (edge == WMSZ_TOP || edge == WMSZ_BOTTOM)
    {
        area->right = area->left + (frame.right - frame.left) +
            (int) (((area->bottom - area->top) - (frame.bottom - frame.top)) * ratio);
    }
}

#endif
