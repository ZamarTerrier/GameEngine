#include "Data/e_resource_engine.h"

#ifdef _WIN_
    #include "wManager/manager_includes.h"
    #include "wManager/input_manager.h"
    #include "wManager/win_defines.h"

extern wManagerWindow _wMWindow;
extern wManagerInfo _wMInfo;

extern BOOL GetErrorMessage(DWORD dwErrorCode, LPTSTR *pBuffer);

extern DWORD getWindowStyle(const wManagerWindow* window);
extern DWORD getWindowExStyle(const wManagerWindow* window);

extern int getKeyMods(void);
extern void applyAspectRatio(wManagerWindow* window, int edge, RECT* area);

extern char* _wManagerCreateUTF8FromWideStringWin32(const WCHAR* source);
extern WCHAR* _wManagerCreateWideStringFromUTF8Win32(const char* source);

extern void _wManagerCenterCursorInContentArea(wManagerWindow* window);


// Update window framebuffer transparency
//
static void updateFramebufferTransparency(const wManagerWindow* window)
{
    /*BOOL composition, opaque;
    DWORD color;

    if (!IsWindowsVistaOrGreater())
        return;

    if (FAILED(DwmIsCompositionEnabled(&composition)) || !composition)
       return;

    if (IsWindows8OrGreater() ||
        (SUCCEEDED(DwmGetColorizationColor(&color, &opaque)) && !opaque))
    {
        HRGN region = CreateRectRgn(0, 0, -1, -1);
        DWM_BLURBEHIND bb = {0};
        bb.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
        bb.hRgnBlur = region;
        bb.fEnable = TRUE;

        DwmEnableBlurBehindWindow(((wManagerWin *)window->WindowData)->handle, &bb);
        DeleteObject(region);
    }
    else
    {
        // HACK: Disable framebuffer transparency on Windows 7 when the
        //       colorization color is opaque, because otherwise the window
        //       contents is blended additively with the previous frame instead
        //       of replacing it
        DWM_BLURBEHIND bb = {0};
        bb.dwFlags = DWM_BB_ENABLE;
        DwmEnableBlurBehindWindow(((wManagerWin *)window->WindowData)->handle, &bb);
    }*/
}

// Enables WM_INPUT messages for the mouse for the specified window
//
void enableRawMouseMotion(wManagerWindow* window)
{
    const RAWINPUTDEVICE rid = { 0x01, 0x02, 0, ((wManagerWin *)window->WindowData)->handle };

    if (!RegisterRawInputDevices(&rid, 1, sizeof(rid)))
    {
        printf("Win32: Failed to register raw input device\n");
    }
}

// Disables WM_INPUT messages for the mouse
//
void disableRawMouseMotion(wManagerWindow* window)
{
    const RAWINPUTDEVICE rid = { 0x01, 0x02, RIDEV_REMOVE, NULL };

    if (!RegisterRawInputDevices(&rid, 1, sizeof(rid)))
    {
        printf("Win32: Failed to remove raw input device\n");
    }
}

int32_t cursorInContentArea(wManagerWindow* window)
{
    RECT area;
    POINT pos;

    if (!GetCursorPos(&pos))
        return false;

    if (WindowFromPoint(pos) != ((wManagerWin *)window->WindowData)->handle)
        return false;

    GetClientRect(((wManagerWin *)window->WindowData)->handle, &area);
    ClientToScreen(((wManagerWin *)window->WindowData)->handle, (POINT*) &area.left);
    ClientToScreen(((wManagerWin *)window->WindowData)->handle, (POINT*) &area.right);

    return PtInRect(&area, pos);
}

extern void _wManagerInputWindowCloseRequest(wManagerWindow* window);

// Notifies shared code that a window has lost or received input focus
//
void _wManagerInputWindowFocus(wManagerWindow* window, int32_t focused)
{

    if (window->callbacks.focus)
        window->callbacks.focus((wManagerWindow*) window, focused);

    if (!focused)
    {
        int key, button;

        for (key = 0;  key <= ENGINE_KEY_LAST;  key++)
        {
            if (window->keys[key] == ENGINE_PRESS)
            {
                const int scancode = ((wManagerWin *)_wMWindow.WindowData)->scancodes[key];
                _wManagerInputKey(window, key, scancode, ENGINE_RELEASE, 0);
            }
        }

        for (button = 0;  button <= ENGINE_MOUSE_BUTTON_LAST;  button++)
        {
            if (window->mouseButtons[button] == ENGINE_PRESS)
                _wManagerInputMouseClick(window, button, ENGINE_RELEASE, 0);
        }
    }
}

// Manually maximize the window, for when SW_MAXIMIZE cannot be used
//
void maximizeWindowManually(wManagerWindow* window)
{
    RECT rect;
    DWORD style;
    MONITORINFO mi = { sizeof(mi) };

    GetMonitorInfoW(MonitorFromWindow(((wManagerWin *)window->WindowData)->handle,
                                      MONITOR_DEFAULTTONEAREST), &mi);

    rect = mi.rcWork;

    if (window->maxwidth != ENGINE_DONT_CARE && window->maxheight != ENGINE_DONT_CARE)
    {
        rect.right = _wManager_min(rect.right, rect.left + window->maxwidth);
        rect.bottom = _wManager_min(rect.bottom, rect.top + window->maxheight);
    }

    style = GetWindowLongW(((wManagerWin *)window->WindowData)->handle, GWL_STYLE);
    style |= WS_MAXIMIZE;
    SetWindowLongW(((wManagerWin *)window->WindowData)->handle, GWL_STYLE, style);

    /*if (window->decorated)
    {
        const DWORD exStyle = GetWindowLongW(((wManagerWin *)window->WindowData)->handle, GWL_EXSTYLE);

        AdjustWindowRectEx(&rect, style, FALSE, exStyle);
        OffsetRect(&rect, 0, GetSystemMetrics(SM_CYCAPTION));

        rect.bottom = _wManager_min(rect.bottom, mi.rcWork.bottom);
    }*/

    SetWindowPos(((wManagerWin *)window->WindowData)->handle, HWND_TOP,
                 rect.left,
                 rect.top,
                 rect.right - rect.left,
                 rect.bottom - rect.top,
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_FRAMECHANGED);
}

int32_t _wManagerWindowFocusedWin32(wManagerWindow* window)
{
    return ((wManagerWin *)window->WindowData)->handle == GetActiveWindow();
}

int32_t _wManagerWindowIconifiedWin32(wManagerWindow* window)
{
    return IsIconic(((wManagerWin *)window->WindowData)->handle);
}

int32_t _wManagerWindowVisibleWin32(wManagerWindow* window)
{
    return IsWindowVisible(((wManagerWin *)window->WindowData)->handle);
}

int32_t _wManagerWindowMaximizedWin32(wManagerWindow* window)
{
    return IsZoomed(((wManagerWin *)window->WindowData)->handle);
}

int32_t _wManagerWindowHoveredWin32(wManagerWindow* window)
{
    return cursorInContentArea(window);
}

int32_t _wManagerFramebufferTransparentWin32(wManagerWindow* window)
{
    BOOL composition, opaque;
    DWORD color;

    /*if (!window->win32.transparent)
        return false;

    if (!IsWindowsVistaOrGreater())
        return false;

    if (FAILED(DwmIsCompositionEnabled(&composition)) || !composition)
        return false;

    if (!IsWindows8OrGreater())
    {
        // HACK: Disable framebuffer transparency on Windows 7 when the
        //       colorization color is opaque, because otherwise the window
        //       contents is blended additively with the previous frame instead
        //       of replacing it
        if (FAILED(DwmGetColorizationColor(&color, &opaque)) || opaque)
            return false;
    }*/

    return true;
}

void _wManagerSetWindowMousePassthroughWin32(wManagerWindow* window, int32_t enabled)
{
    COLORREF key = 0;
    BYTE alpha = 0;
    DWORD flags = 0;
    DWORD exStyle = GetWindowLongW(((wManagerWin *)window->WindowData)->handle, GWL_EXSTYLE);

    if (exStyle & WS_EX_LAYERED)
        GetLayeredWindowAttributes(((wManagerWin *)window->WindowData)->handle, &key, &alpha, &flags);

    if (enabled)
        exStyle |= (WS_EX_TRANSPARENT | WS_EX_LAYERED);
    else
    {
        exStyle &= ~WS_EX_TRANSPARENT;
        // NOTE: Window opacity also needs the layered window style so do not
        //       remove it if the window is alpha blended
        if (exStyle & WS_EX_LAYERED)
        {
            if (!(flags & LWA_ALPHA))
                exStyle &= ~WS_EX_LAYERED;
        }
    }

    SetWindowLongW(((wManagerWin *)window->WindowData)->handle, GWL_EXSTYLE, exStyle);

    if (enabled)
        SetLayeredWindowAttributes(((wManagerWin *)window->WindowData)->handle, key, alpha, flags);
}

float _wManagerGetWindowOpacityWin32(wManagerWindow* window)
{
    BYTE alpha;
    DWORD flags;

    if ((GetWindowLongW(((wManagerWin *)window->WindowData)->handle, GWL_EXSTYLE) & WS_EX_LAYERED) &&
        GetLayeredWindowAttributes(((wManagerWin *)window->WindowData)->handle, NULL, &alpha, &flags))
    {
        if (flags & LWA_ALPHA)
            return alpha / 255.f;
    }

    return 1.f;
}

void _wManagerSetWindowOpacityWin32(wManagerWindow* window, float opacity)
{
    LONG exStyle = GetWindowLongW(((wManagerWin *)window->WindowData)->handle, GWL_EXSTYLE);
    if (opacity < 1.f || (exStyle & WS_EX_TRANSPARENT))
    {
        const BYTE alpha = (BYTE) (255 * opacity);
        exStyle |= WS_EX_LAYERED;
        SetWindowLongW(((wManagerWin *)window->WindowData)->handle, GWL_EXSTYLE, exStyle);
        SetLayeredWindowAttributes(((wManagerWin *)window->WindowData)->handle, 0, alpha, LWA_ALPHA);
    }
    else if (exStyle & WS_EX_TRANSPARENT)
    {
        SetLayeredWindowAttributes(((wManagerWin *)window->WindowData)->handle, 0, 0, 0);
    }
    else
    {
        exStyle &= ~WS_EX_LAYERED;
        SetWindowLongW(((wManagerWin *)window->WindowData)->handle, GWL_EXSTYLE, exStyle);
    }
}


// Update native window styles to match attributes
//
void updateWindowStyles(const wManagerWindow* window)
{
    RECT rect;
    DWORD style = GetWindowLongW(((wManagerWin *)window->WindowData)->handle, GWL_STYLE);
    style &= ~(WS_OVERLAPPEDWINDOW | WS_POPUP);
    style |= getWindowStyle(window);

    GetClientRect(((wManagerWin *)window->WindowData)->handle, &rect);

    AdjustWindowRectEx(&rect, style, FALSE, getWindowExStyle(window));

    ClientToScreen(((wManagerWin *)window->WindowData)->handle, (POINT*) &rect.left);
    ClientToScreen(((wManagerWin *)window->WindowData)->handle, (POINT*) &rect.right);
    SetWindowLongW(((wManagerWin *)window->WindowData)->handle, GWL_STYLE, style);
    SetWindowPos(((wManagerWin *)window->WindowData)->handle, HWND_TOP,
                 rect.left, rect.top,
                 rect.right - rect.left, rect.bottom - rect.top,
                 SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOZORDER);
}


// Updates the cursor image according to its cursor mode
//
void updateCursorImage(wManagerWindow *window)
{
    if (window->cursorMode == ENGINE_CURSOR_NORMAL ||
        window->cursorMode == ENGINE_CURSOR_CAPTURED)
    {
        /*if (window->cursor)
            SetCursor(window->cursor->win32.handle);
        else*/
            SetCursor(LoadCursorW(NULL, IDC_ARROW));
    }
    else
        //Connected via Remote Desktop, NULL cursor will present SetCursorPos the move the cursor.
        //using a blank cursor fix that.
        //When not via Remote Desktop, win32.blankCursor should be NULL
        SetCursor(NULL);

    //SetCursor(LoadCursorW(NULL, IDC_ARROW));
}

// Sets the cursor clip rect to the window content area
//
void captureCursor(wManagerWindow *window)
{
    RECT clipRect;
    GetClientRect(((wManagerWin *)window->WindowData)->handle, &clipRect);
    ClientToScreen(((wManagerWin *)window->WindowData)->handle, (POINT*) &clipRect.left);
    ClientToScreen(((wManagerWin *)window->WindowData)->handle, (POINT*) &clipRect.right);
    ClipCursor(&clipRect);
    ((wManagerWin *)window->WindowData)->capturedCursorWindow = window;
}

// Disabled clip cursor
//
void releaseCursor(wManagerWindow *window)
{
    ClipCursor(NULL);
    ((wManagerWin *)window->WindowData)->capturedCursorWindow = NULL;
}

void _wManagerSetCursorPosWin32(wManagerWindow *window, double xpos, double ypos)
{
    POINT pos = { (int) xpos, (int) ypos };

    // Store the new position so it can be recognized later
    ((wManagerWin* )window->WindowData)->lastCursorPosX = pos.x;
    ((wManagerWin* )window->WindowData)->lastCursorPosY = pos.y;

    ClientToScreen(((wManagerWin *)window->WindowData)->handle, &pos);
    SetCursorPos(pos.x, pos.y);
}

void _wManagerGetCursorPosWin32(wManagerWindow *window, double* xpos, double* ypos)
{
    POINT pos;

    if (GetCursorPos(&pos))
    {
        ScreenToClient(((wManagerWin *)window->WindowData)->handle, &pos);

        if (xpos)
            *xpos = pos.x;
        if (ypos)
            *ypos = pos.y;
    }
}

void _wManagerSetWindowResizableWin32(wManagerWindow* window, int32_t enabled)
{
    updateWindowStyles(window);
}

void _wManagerSetWindowDecoratedWin32(wManagerWindow* window, int32_t enabled)
{
    updateWindowStyles(window);
}

void _wManagerSetWindowFloatingWin32(wManagerWindow* window, int32_t enabled)
{
    const HWND after = enabled ? HWND_TOPMOST : HWND_NOTOPMOST;
    SetWindowPos(((wManagerWin *)window->WindowData)->handle, after, 0, 0, 0, 0,
                 SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
}


void _wManagerSetRawMouseMotionWin32(wManagerWindow *window, int32_t enabled)
{
    /*if (_wMWindow.disabledCursorWindow != window)
        return;*/

    if (enabled)
        enableRawMouseMotion(window);
    else
        disableRawMouseMotion(window);
}

int32_t _wManagerRawMouseMotionSupportedWin32(void)
{
    return true;
}

const char* _wManagerGetScancodeNameWin32(int scancode)
{
    if (scancode < 0 || scancode > (KF_EXTENDED | 0xff))
    {
        printf("Invalid scancode %i\n", scancode);
        return NULL;
    }

    const int key = ((wManagerWin* )_wMWindow.WindowData)->keycodes[scancode];
    if (key == ENGINE_KEY_UNKNOWN)
        return NULL;

    return ((wManagerWin* )_wMWindow.WindowData)->keynames[key];
}

int _wManagerGetKeyScancodeWin32(int key)
{
    return ((wManagerWin* )_wMWindow.WindowData)->scancodes[key];
}

void _wManagerSetClipboardStringWin32(const char* string)
{
    int characterCount, tries = 0;
    HANDLE object;
    WCHAR* buffer;

    characterCount = MultiByteToWideChar(CP_UTF8, 0, string, -1, NULL, 0);
    if (!characterCount)
        return;

    object = GlobalAlloc(GMEM_MOVEABLE, characterCount * sizeof(WCHAR));
    if (!object)
    {
        printf("Win32: Failed to allocate global handle for clipboard\n");
        return;
    }

    buffer = GlobalLock(object);
    if (!buffer)
    {
        printf("Win32: Failed to lock global handle\n");
        GlobalFree(object);
        return;
    }

    MultiByteToWideChar(CP_UTF8, 0, string, -1, buffer, characterCount);
    GlobalUnlock(object);

    // NOTE: Retry clipboard opening a few times as some other application may have it
    //       open and also the Windows Clipboard History reads it after each update
    /*while (!OpenClipboard(((wManagerWin* )_wMWindow.WindowData)->helperWindowHandle))
    {
        Sleep(1);
        tries++;

        if (tries == 3)
        {
            printf("Win32: Failed to open clipboard\n");
            GlobalFree(object);
            return;
        }
    }*/

    EmptyClipboard();
    SetClipboardData(CF_UNICODETEXT, object);
    CloseClipboard();
}

const char* _wManagerGetClipboardStringWin32(void)
{
    HANDLE object;
    WCHAR* buffer;
    int tries = 0;

    // NOTE: Retry clipboard opening a few times as some other application may have it
    //       open and also the Windows Clipboard History reads it after each update
    /*while (!OpenClipboard(((wManagerWin* )_wMWindow.WindowData)->helperWindowHandle))
    {
        Sleep(1);
        tries++;

        if (tries == 3)
        {
            _glfwInputErrorWin32(ENGINE_PLATFORM_ERROR,
                                 "Win32: Failed to open clipboard");
            return NULL;
        }
    }*/

    object = GetClipboardData(CF_UNICODETEXT);
    if (!object)
    {
        printf("Win32: Failed to convert clipboard to string\n");
        CloseClipboard();
        return NULL;
    }

    buffer = GlobalLock(object);
    if (!buffer)
    {
        printf("Win32: Failed to lock global handle\n");
        CloseClipboard();
        return NULL;
    }

    free(((wManagerWin* )_wMWindow.WindowData)->clipboardString);
    ((wManagerWin* )_wMWindow.WindowData)->clipboardString = _wManagerCreateUTF8FromWideStringWin32(buffer);

    GlobalUnlock(object);
    CloseClipboard();

    return ((wManagerWin* )_wMWindow.WindowData)->clipboardString;
}

void _wManagerGetWindowSizeWin32(wManagerWindow *window, int* width, int* height)
{
    RECT area;
    GetClientRect(((wManagerWin *)window->WindowData)->handle, &area);

    if (width)
        *width = area.right;
    if (height)
        *height = area.bottom;
}

void _wManagerSetWindowSizeWin32(wManagerWindow* window, int width, int height)
{
    /*if (window->monitor)
    {
        if (window->monitor->window == window)
        {
            acquireMonitor(window);
            fitToMonitor(window);
        }
    }
    else*/
    {
        RECT rect = { 0, 0, width, height };

        AdjustWindowRectEx(&rect, getWindowStyle(window),
                           FALSE, getWindowExStyle(window));

        SetWindowPos(((wManagerWin *)window->WindowData)->handle, HWND_TOP,
                     0, 0, rect.right - rect.left, rect.bottom - rect.top,
                     SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOZORDER);
    }
}

void _wManagerSetWindowSizeLimitsWin32(wManagerWindow* window,
                                   int minwidth, int minheight,
                                   int maxwidth, int maxheight)
{
    RECT area;

    if ((minwidth == ENGINE_DONT_CARE || minheight == ENGINE_DONT_CARE) &&
        (maxwidth == ENGINE_DONT_CARE || maxheight == ENGINE_DONT_CARE))
    {
        return;
    }

    GetWindowRect(((wManagerWin *)window->WindowData)->handle, &area);
    MoveWindow(((wManagerWin *)window->WindowData)->handle,
               area.left, area.top,
               area.right - area.left,
               area.bottom - area.top, TRUE);
}

void _wManagerSetWindowAspectRatioWin32(wManagerWindow* window, int numer, int denom)
{
    RECT area;

    if (numer == ENGINE_DONT_CARE || denom == ENGINE_DONT_CARE)
        return;

    GetWindowRect(((wManagerWin *)window->WindowData)->handle, &area);
    applyAspectRatio(window, WMSZ_BOTTOMRIGHT, &area);
    MoveWindow(((wManagerWin *)window->WindowData)->handle,
               area.left, area.top,
               area.right - area.left,
               area.bottom - area.top, TRUE);
}

void _wManagerGetFramebufferSizeWin32(wManagerWindow* window, int* width, int* height)
{
    _wManagerGetWindowSizeWin32(window, width, height);
}

void _wManagerGetWindowFrameSizeWin32(wManagerWindow* window,
                                  int* left, int* top,
                                  int* right, int* bottom)
{
    RECT rect;
    int width, height;

    _wManagerGetWindowSizeWin32(window, &width, &height);
    SetRect(&rect, 0, 0, width, height);


    AdjustWindowRectEx(&rect, getWindowStyle(window),
                       FALSE, getWindowExStyle(window));

    if (left)
        *left = -rect.left;
    if (top)
        *top = -rect.top;
    if (right)
        *right = rect.right - width;
    if (bottom)
        *bottom = rect.bottom - height;
}

void _wManagerGetWindowContentScaleWin32(wManagerWindow* window, float* xscale, float* yscale)
{
    const HANDLE handle = MonitorFromWindow(((wManagerWin *)window->WindowData)->handle,
                                            MONITOR_DEFAULTTONEAREST);
    //_wManagerGetHMONITORContentScaleWin32(handle, xscale, yscale);
}

void _wManagerIconifyWindowWin32(wManagerWindow* window)
{
    ShowWindow(((wManagerWin *)window->WindowData)->handle, SW_MINIMIZE);
}

void _wManagerRestoreWindowWin32(wManagerWindow* window)
{
    ShowWindow(((wManagerWin *)window->WindowData)->handle, SW_RESTORE);
}

void _wManagerMaximizeWindowWin32(wManagerWindow* window)
{
    if (IsWindowVisible(((wManagerWin *)window->WindowData)->handle))
        ShowWindow(((wManagerWin *)window->WindowData)->handle, SW_MAXIMIZE);
    else
        maximizeWindowManually(window);
}

void _wManagerShowWindowWin32(wManagerWindow* window)
{
    ShowWindow(((wManagerWin *)window->WindowData)->handle, SW_SHOWNA);
}

void _wManagerHideWindowWin32(wManagerWindow* window)
{
    ShowWindow(((wManagerWin *)window->WindowData)->handle, SW_HIDE);
}

void _wManagerRequestWindowAttentionWin32(wManagerWindow* window)
{
    FlashWindow(((wManagerWin *)window->WindowData)->handle, TRUE);
}

void _wManagerFocusWindowWin32(wManagerWindow* window)
{
    BringWindowToTop(((wManagerWin *)window->WindowData)->handle);
    SetForegroundWindow(((wManagerWin *)window->WindowData)->handle);
    SetFocus(((wManagerWin *)window->WindowData)->handle);
}

void _wManagerSetCursorModeWin32(wManagerWindow* window, int mode)
{
    if (_wManagerWindowFocusedWin32(window))
    {
        if (mode == ENGINE_CURSOR_DISABLED)
        {
            _wManagerGetCursorPosWin32(window,
                                   &((wManagerWin *)_wMWindow.WindowData)->restoreCursorPosX,
                                   &((wManagerWin *)_wMWindow.WindowData)->restoreCursorPosY);
            //_wManagerCenterCursorInContentArea(window);
            if (window->rawMouseMotion)
                enableRawMouseMotion(window);
        }
        else if (((wManagerWin *)_wMWindow.WindowData)->disabledCursorWindow == window)
        {
            if (window->rawMouseMotion)
                disableRawMouseMotion(window);
        }

        if (mode == ENGINE_CURSOR_DISABLED || mode == ENGINE_CURSOR_CAPTURED)
            captureCursor(window);
        else
            releaseCursor(window);

        if (mode == ENGINE_CURSOR_DISABLED)
            ((wManagerWin *)_wMWindow.WindowData)->disabledCursorWindow = window;
        else if (((wManagerWin *)_wMWindow.WindowData)->disabledCursorWindow == window)
        {
            ((wManagerWin *)_wMWindow.WindowData)->disabledCursorWindow = NULL;
            _wManagerSetCursorPosWin32(window,
                                   ((wManagerWin *)_wMWindow.WindowData)->restoreCursorPosX,
                                   ((wManagerWin *)_wMWindow.WindowData)->restoreCursorPosY);
        }
    }

    if (cursorInContentArea(window))
        updateCursorImage(window);
}

// Apply disabled cursor mode to a focused window
//
void disableCursor(wManagerWindow *window)
{
    ((wManagerWin *)window->WindowData)->disabledCursorWindow = window;
    _wManagerGetCursorPosWin32(window, &((wManagerWin *)window->WindowData)->restoreCursorPosX,
                               &((wManagerWin *)window->WindowData)->restoreCursorPosY);
    updateCursorImage(window);

    int width, height;

    _wManagerGetWindowSizeWin32(window, &width, &height);
    _wManagerSetCursorPosWin32(window, width / 2.0f, height / 2.0f);

    captureCursor(window);

    if (window->rawMouseMotion)
        enableRawMouseMotion(window);
}

// Exit disabled cursor mode for the specified window
//
void enableCursor(wManagerWindow *window)
{
    if (window->rawMouseMotion)
        disableRawMouseMotion(window);

    ((wManagerWin *)window->WindowData)->disabledCursorWindow = NULL;
    releaseCursor(window);
    _wManagerSetCursorPosWin32(window, ((wManagerWin *)window->WindowData)->restoreCursorPosX,
                               ((wManagerWin *)window->WindowData)->restoreCursorPosY);
    updateCursorImage(window);
}

extern void _wManagerInputWindowDamage(wManagerWindow* window);
extern void _wManagerInputWindowPos(wManagerWindow* window, int x, int y);

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {

    wManagerWindow* window = e_window;

    if (!window)
    {
        return DefWindowProcW(hWnd, uMsg, wParam, lParam);
    }

    switch (uMsg)
    {
        case WM_MOUSEACTIVATE:
        {
            // HACK: Postpone cursor disabling when the window was activated by
            //       clicking a caption button
            if (HIWORD(lParam) == WM_LBUTTONDOWN)
            {
                if (LOWORD(lParam) != HTCLIENT)
                    _wMWindow.frameAction = true;
            }

            break;
        }

        case WM_CAPTURECHANGED:
        {
            // HACK: Disable the cursor once the caption button action has been
            //       completed or cancelled
            if (lParam == 0 && _wMWindow.frameAction)
            {
                if (window->cursorMode == ENGINE_CURSOR_DISABLED)
                    disableCursor(window);
                else if (window->cursorMode == ENGINE_CURSOR_CAPTURED)
                    captureCursor(window);

                _wMWindow.frameAction = false;
            }

            break;
        }

        case WM_SETFOCUS:
        {
            _wManagerInputWindowFocus(window, true);

            // HACK: Do not disable cursor while the user is interacting with
            //       a caption button
            if (window->frameAction)
                break;

            if (window->cursorMode == ENGINE_CURSOR_DISABLED)
                disableCursor(window);
            else if (window->cursorMode == ENGINE_CURSOR_CAPTURED)
                captureCursor(window);

            return 0;
        }

        case WM_KILLFOCUS:
        {
            if (window->cursorMode == ENGINE_CURSOR_DISABLED)
                enableCursor(window);
            else if (window->cursorMode == ENGINE_CURSOR_CAPTURED)
                releaseCursor(window);

            //if (_wMWindow->monitor && window->hints.window.autoIconify)
             //   _wManagerIconifyWindowWin32(window);

            _wManagerInputWindowFocus(window, false);
            return 0;
        }

//        case WM_SYSCOMMAND:
//        {
//            switch (wParam & 0xfff0)
//            {
//    //                case SC_SCREENSAVE:
//    //                case SC_MONITORPOWER:
//    //                {
//    //                    if (window->monitor)
//    //                    {
//    //                        // We are running in full screen mode, so disallow
//    //                        // screen saver and screen blanking
//    //                        return 0;
//    //                    }
//    //                    else
//    //                        break;
//    //                }

//                // User trying to access application menu using ALT?
//                case SC_KEYMENU:
//                {
//                    if (!window->keymenu)
//                        return 0;

//                    break;
//                }
//            }
//            break;
//        }

        case WM_CLOSE:
        {
            _wMWindow.shouldClose = true;
            return 0;
        }


//        case WM_INPUTLANGCHANGE:
//        {
//            _wMInfoUpdateKeyNamesWin32();
//            break;
//        }

//        case WM_CHAR:
//        case WM_SYSCHAR:
//        {
//            if (wParam >= 0xd800 && wParam <= 0xdbff)
//                window->highSurrogate = (WCHAR) wParam;
//            else
//            {
//                uint32_t codepoint = 0;

//                if (wParam >= 0xdc00 && wParam <= 0xdfff)
//                {
//                    if (window->highSurrogate)
//                    {
//                        codepoint += (window->highSurrogate - 0xd800) << 10;
//                        codepoint += (WCHAR) wParam - 0xdc00;
//                        codepoint += 0x10000;
//                    }
//                }
//                else
//                    codepoint = (WCHAR) wParam;

//                window->highSurrogate = 0;
//                _wManagerInputChar(window, codepoint, getKeyMods(), uMsg != WM_SYSCHAR);
//            }

//            if (uMsg == WM_SYSCHAR && window->hints.window.win32.keymenu)
//                break;

//            return 0;
//        }

//        case WM_UNICHAR:
//        {
//            if (wParam == UNICODE_NOCHAR)
//            {
//                // WM_UNICHAR is not sent by Windows, but is sent by some
//                // third-party input method engine
//                // Returning TRUE here announces support for this message
//                return true;
//            }

//            _wManagerInputChar(window, (uint32_t) wParam, getKeyMods(), true);
//            return 0;
//        }

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            int key, scancode;
            const int action = (HIWORD(lParam) & KF_UP) ? ENGINE_RELEASE : ENGINE_PRESS;
            const int mods = getKeyMods();

            scancode = (HIWORD(lParam) & (KF_EXTENDED | 0xff));
            if (!scancode)
            {
                // NOTE: Some synthetic key messages have a scancode of zero
                // HACK: Map the virtual key back to a usable scancode
                scancode = MapVirtualKeyW((UINT) wParam, MAPVK_VK_TO_VSC);
            }

            // HACK: Alt+PrtSc has a different scancode than just PrtSc
            if (scancode == 0x54)
                scancode = 0x137;

            // HACK: Ctrl+Pause has a different scancode than just Pause
            if (scancode == 0x146)
                scancode = 0x45;

            // HACK: CJK IME sets the extended bit for right Shift
            if (scancode == 0x136)
                scancode = 0x36;

            key = ((wManagerWin* )_wMWindow.WindowData)->keycodes[scancode];

            // The Ctrl keys require special handling
            if (wParam == VK_CONTROL)
            {
                if (HIWORD(lParam) & KF_EXTENDED)
                {
                    // Right side keys have the extended key bit set
                    key = ENGINE_KEY_RIGHT_CONTROL;
                }
                else
                {
                    // NOTE: Alt Gr sends Left Ctrl followed by Right Alt
                    // HACK: We only want one event for Alt Gr, so if we detect
                    //       this sequence we discard this Left Ctrl message now
                    //       and later report Right Alt normally
                    MSG next;
                    const DWORD time = GetMessageTime();

                    if (PeekMessageW(&next, NULL, 0, 0, PM_NOREMOVE))
                    {
                        if (next.message == WM_KEYDOWN ||
                            next.message == WM_SYSKEYDOWN ||
                            next.message == WM_KEYUP ||
                            next.message == WM_SYSKEYUP)
                        {
                            if (next.wParam == VK_MENU &&
                                (HIWORD(next.lParam) & KF_EXTENDED) &&
                                next.time == time)
                            {
                                // Next message is Right Alt down so discard this
                                break;
                            }
                        }
                    }

                    // This is a regular Left Ctrl message
                    key = ENGINE_KEY_LEFT_CONTROL;
                }
            }
            else if (wParam == VK_PROCESSKEY)
            {
                // IME notifies that keys have been filtered by setting the
                // virtual key-code to VK_PROCESSKEY
                break;
            }

            if (action == ENGINE_RELEASE && wParam == VK_SHIFT)
            {
                // HACK: Release both Shift keys on Shift up event, as when both
                //       are pressed the first release does not emit any event
                // NOTE: The other half of this is in _wMInfoPollEventsWin32
                _wManagerInputKey(window, ENGINE_KEY_LEFT_SHIFT, scancode, action, mods);
                _wManagerInputKey(window, ENGINE_KEY_RIGHT_SHIFT, scancode, action, mods);
            }
            else if (wParam == VK_SNAPSHOT)
            {
                // HACK: Key down is not reported for the Print Screen key
                _wManagerInputKey(window, key, scancode, ENGINE_PRESS, mods);
                _wManagerInputKey(window, key, scancode, ENGINE_RELEASE, mods);
            }
            else
                _wManagerInputKey(window, key, scancode, action, mods);

            break;
        }

//        case WM_LBUTTONDOWN:
//        case WM_RBUTTONDOWN:
//        case WM_MBUTTONDOWN:
//        case WM_XBUTTONDOWN:
//        case WM_LBUTTONUP:
//        case WM_RBUTTONUP:
//        case WM_MBUTTONUP:
//        case WM_XBUTTONUP:
//        {
//            int i, button, action;

//            if (uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP)
//                button = ENGINE_MOUSE_BUTTON_LEFT;
//            else if (uMsg == WM_RBUTTONDOWN || uMsg == WM_RBUTTONUP)
//                button = ENGINE_MOUSE_BUTTON_RIGHT;
//            else if (uMsg == WM_MBUTTONDOWN || uMsg == WM_MBUTTONUP)
//                button = ENGINE_MOUSE_BUTTON_MIDDLE;
//            else if (GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
//                button = ENGINE_MOUSE_BUTTON_4;
//            else
//                button = ENGINE_MOUSE_BUTTON_5;

//            if (uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN ||
//                uMsg == WM_MBUTTONDOWN || uMsg == WM_XBUTTONDOWN)
//            {
//                action = ENGINE_PRESS;
//            }
//            else
//                action = ENGINE_RELEASE;

//            for (i = 0;  i <= ENGINE_MOUSE_BUTTON_LAST;  i++)
//            {
//                if (window->mouseButtons[i] == ENGINE_PRESS)
//                    break;
//            }

//            if (i > ENGINE_MOUSE_BUTTON_LAST)
//                SetCapture(hWnd);

//            _wManagerInputMouseClick(window, button, action, getKeyMods());

//            for (i = 0;  i <= ENGINE_MOUSE_BUTTON_LAST;  i++)
//            {
//                if (window->mouseButtons[i] == ENGINE_PRESS)
//                    break;
//            }

//            if (i > ENGINE_MOUSE_BUTTON_LAST)
//                ReleaseCapture();

//            if (uMsg == WM_XBUTTONDOWN || uMsg == WM_XBUTTONUP)
//                return TRUE;

//            return 0;
//        }

        case WM_MOUSEMOVE:
        {
            const int x = (int)(short) LOWORD(lParam);
            const int y = (int)(short) HIWORD(lParam);

            if (!window->cursorTracked)
            {
                TRACKMOUSEEVENT tme;
                ZeroMemory(&tme, sizeof(tme));
                tme.cbSize = sizeof(tme);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = ((wManagerWin *)window->WindowData)->handle;
                TrackMouseEvent(&tme);

                window->cursorTracked = true;
                _wManagerInputCursorEnter(window, true);
            }

            if (window->cursorMode == ENGINE_CURSOR_DISABLED)
            {
                const int dx = x - ((wManagerWin* )window->WindowData)->lastCursorPosX;
                const int dy = y - ((wManagerWin* )window->WindowData)->lastCursorPosY;

                if (((wManagerWin *)_wMWindow.WindowData)->disabledCursorWindow != window)
                    break;
                if (window->rawMouseMotion)
                    break;

                _wManagerInputCursorPos(window, ((wManagerWin* )window->WindowData)->virtualCursorPosX + dx,
                                                ((wManagerWin* )window->WindowData)->virtualCursorPosY + dy);
            }
            else
                _wManagerInputCursorPos(window, x, y);

            ((wManagerWin* )window->WindowData)->lastCursorPosX = x;
            ((wManagerWin* )window->WindowData)->lastCursorPosY = y;

            return 0;
        }

        case WM_INPUT:
        {
            UINT size = 0;
            HRAWINPUT ri = (HRAWINPUT) lParam;
            RAWINPUT* data = NULL;
            int dx, dy;
            int width, height;
            POINT pos;

            if (((wManagerWin *)_wMWindow.WindowData)->disabledCursorWindow != window)
                break;
            if (!window->rawMouseMotion)
                break;

            GetRawInputData(ri, RID_INPUT, NULL, &size, sizeof(RAWINPUTHEADER));
            if (size > (UINT) ((wManagerWin *)window->WindowData)->rawInputSize)
            {
                free(((wManagerWin *)window->WindowData)->rawInput);
                ((wManagerWin *)window->WindowData)->rawInput = calloc(size, 1);
                ((wManagerWin *)window->WindowData)->rawInputSize = size;
            }

            size = ((wManagerWin *)window->WindowData)->rawInputSize;
            if (GetRawInputData(ri, RID_INPUT,
                                ((wManagerWin *)window->WindowData)->rawInput, &size,
                                sizeof(RAWINPUTHEADER)) == (UINT) -1)
            {
                printf("Win32: Failed to retrieve raw input data\n");
                break;
            }

            data = ((wManagerWin *)window->WindowData)->rawInput;
            if (data->data.mouse.usFlags & MOUSE_MOVE_ABSOLUTE)
            {
                if (_wMWindow.isRemoteSession)
                {
                    //Remote Desktop Mode...
                    // As per https://github.com/Microsoft/DirectXTK/commit/ef56b63f3739381e451f7a5a5bd2c9779d2a7555
                    // MOUSE_MOVE_ABSOLUTE is a range from 0 through 65535, based on the screen size.
                    // As far as I can tell, absolute mode only occurs over RDP though.
                    width = GetSystemMetrics((data->data.mouse.usFlags & MOUSE_VIRTUAL_DESKTOP) ? SM_CXVIRTUALSCREEN : SM_CXSCREEN);
                    height = GetSystemMetrics((data->data.mouse.usFlags & MOUSE_VIRTUAL_DESKTOP) ? SM_CYVIRTUALSCREEN : SM_CYSCREEN);

                    pos.x = (int)((data->data.mouse.lLastX / 65535.0f) * width);
                    pos.y = (int)((data->data.mouse.lLastY / 65535.0f) * height);
                    ScreenToClient(((wManagerWin *)window->WindowData)->handle, &pos);

                    dx = pos.x - ((wManagerWin* )window->WindowData)->lastCursorPosX;
                    dy = pos.y - ((wManagerWin* )window->WindowData)->lastCursorPosY;
                }
                else
                {
                    //Normal mode... We should have the right absolute coords in data.mouse
                    dx = data->data.mouse.lLastX - ((wManagerWin* )window->WindowData)->lastCursorPosX;
                    dy = data->data.mouse.lLastY - ((wManagerWin* )window->WindowData)->lastCursorPosY;
                }
            }
            else
            {
                dx = data->data.mouse.lLastX;
                dy = data->data.mouse.lLastY;
            }

            _wManagerInputCursorPos(window, ((wManagerWin* )window->WindowData)->virtualCursorPosX + dx,
                                            ((wManagerWin* )window->WindowData)->virtualCursorPosY + dy);

            ((wManagerWin* )window->WindowData)->lastCursorPosX += dx;
            ((wManagerWin* )window->WindowData)->lastCursorPosY += dy;
            break;
        }

        case WM_MOUSELEAVE:
        {
            window->cursorTracked = false;
            _wManagerInputCursorEnter(window, false);
            return 0;
        }

        case WM_MOUSEWHEEL:
        {
            _wManagerInputScroll(window, 0.0, (SHORT) HIWORD(wParam) / (double) WHEEL_DELTA);
            return 0;
        }

//        case WM_MOUSEHWHEEL:
//        {
//            // This message is only sent on Windows Vista and later
//            // NOTE: The X-axis is inverted for consistency with macOS and X11
//            _wManagerInputScroll(window, -((SHORT) HIWORD(wParam) / (double) WHEEL_DELTA), 0.0);
//            return 0;
//        }

//        case WM_ENTERSIZEMOVE:
//        case WM_ENTERMENULOOP:
//        {
//            if (window->frameAction)
//                break;

//            // HACK: Enable the cursor while the user is moving or
//            //       resizing the window or using the window menu
//            if (((wManagerWin* )window->WindowData)->cursorMode == ENGINE_CURSOR_DISABLED)
//                enableCursor(window);
//            else if (((wManagerWin* )window->WindowData)->cursorMode == ENGINE_CURSOR_CAPTURED)
//                releaseCursor(window);

//            break;
//        }

//        case WM_EXITSIZEMOVE:
//        case WM_EXITMENULOOP:
//        {
//            if (window->frameAction)
//                break;

//            // HACK: Disable the cursor once the user is done moving or
//            //       resizing the window or using the menu
//            if (((wManagerWin* )window->WindowData)->cursorMode == ENGINE_CURSOR_DISABLED)
//                disableCursor(window);
//            else if (((wManagerWin* )window->WindowData)->cursorMode == ENGINE_CURSOR_CAPTURED)
//                captureCursor(window);

//            break;
//        }

//        case WM_SIZE:
//        {
//            const int width = LOWORD(lParam);
//            const int height = HIWORD(lParam);
//            const int32_t iconified = wParam == SIZE_MINIMIZED;
//            const int32_t maximized = wParam == SIZE_MAXIMIZED ||
//                                       (window->maximized &&
//                                        wParam != SIZE_RESTORED);

//            if (_wMWindow.capturedCursorWindow == window)
//                captureCursor(window);

//            if (window->iconified != iconified)
//                _wManagerInputWindowIconify(window, iconified);

//            if (window->maximized != maximized)
//                _wManagerInputWindowMaximize(window, maximized);

//            if (width != window->width || height != window->height)
//            {
//                window->width = width;
//                window->height = height;

//                _wManagerInputFramebufferSize(window, width, height);
//                _wManagerInputWindowSize(window, width, height);
//            }

////            if (window->monitor && ((wManagerWin *)window->WindowData)->iconified != iconified)
////            {
////                if (iconified)
////                    releaseMonitor(window);
////                else
////                {
////                    acquireMonitor(window);
////                    fitToMonitor(window);
////                }
////            }

//            window->iconified = iconified;
//            window->maximized = maximized;
//            return 0;
//        }

        case WM_MOVE:
        {
            if (((wManagerWin *)_wMWindow.WindowData)->capturedCursorWindow == window)
                captureCursor(window);

            // NOTE: This cannot use LOWORD/HIWORD recommended by MSDN, as
            // those macros do not handle negative window positions correctly
            _wManagerInputWindowPos(window,
                                LOWORD(lParam),
                                HIWORD(lParam));
            return 0;
        }

//        case WM_SIZING:
//        {
//            if (window->numer == ENGINE_DONT_CARE ||
//                window->denom == ENGINE_DONT_CARE)
//            {
//                break;
//            }

//            applyAspectRatio(window, (int) wParam, (RECT*) lParam);
//            return TRUE;
//        }

//        case WM_GETMINMAXINFO:
//        {
//            RECT frame = {0};
//            MINMAXINFO* mmi = (MINMAXINFO*) lParam;
//            const DWORD style = getWindowStyle(window);
//            const DWORD exStyle = getWindowExStyle(window);

////            if (window->monitor)
////                break;

//            AdjustWindowRectEx(&frame, style, FALSE, exStyle);

//            if (window->minwidth != ENGINE_DONT_CARE &&
//                window->minheight != ENGINE_DONT_CARE)
//            {
//                mmi->ptMinTrackSize.x = window->minwidth + frame.right - frame.left;
//                mmi->ptMinTrackSize.y = window->minheight + frame.bottom - frame.top;
//            }

//            if (window->maxwidth != ENGINE_DONT_CARE &&
//                window->maxheight != ENGINE_DONT_CARE)
//            {
//                mmi->ptMaxTrackSize.x = window->maxwidth + frame.right - frame.left;
//                mmi->ptMaxTrackSize.y = window->maxheight + frame.bottom - frame.top;
//            }

//            if (!window->hints.window.decorated)
//            {
//                MONITORINFO mi;
//                const HMONITOR mh = MonitorFromWindow(((wManagerWin *)window->WindowData)->handle,
//                                                      MONITOR_DEFAULTTONEAREST);

//                ZeroMemory(&mi, sizeof(mi));
//                mi.cbSize = sizeof(mi);
//                GetMonitorInfoW(mh, &mi);

//                mmi->ptMaxPosition.x = mi.rcWork.left - mi.rcMonitor.left;
//                mmi->ptMaxPosition.y = mi.rcWork.top - mi.rcMonitor.top;
//                mmi->ptMaxSize.x = mi.rcWork.right - mi.rcWork.left;
//                mmi->ptMaxSize.y = mi.rcWork.bottom - mi.rcWork.top;
//            }

//            return 0;
//        }

        case WM_PAINT:
        {
            _wManagerInputWindowDamage(window);
            break;
        }

//        case WM_ERASEBKGND:
//        {
//            return true;
//        }
//        case WM_NCACTIVATE:
//        case WM_NCPAINT:
//        {
//            // Prevent title bar from being drawn after restoring a minimized
//            // undecorated window
//            if (!window->hints.window.decorated)
//                return true;

//            break;
//        }

//        case WM_DWMCOMPOSITIONCHANGED:
//        case WM_DWMCOLORIZATIONCOLORCHANGED:
//        {
//            if (((wManagerWin *)window->WindowData)->transparent)
//                updateFramebufferTransparency(window);
//            return 0;
//        }

//        case WM_GETDPISCALEDSIZE:
//        {
//            if (window->scaleToMonitor)
//                break;

//            // Adjust the window size to keep the content area size constant
//            if (_wMInfoIsWindows10Version1703OrGreaterWin32())
//            {
//                RECT source = {0}, target = {0};
//                SIZE* size = (SIZE*) lParam;

//                AdjustWindowRectExForDpi(&source, getWindowStyle(window),
//                                         FALSE, getWindowExStyle(window),
//                                         GetDpiForWindow(((wManagerWin *)window->WindowData)->handle));
//                AdjustWindowRectExForDpi(&target, getWindowStyle(window),
//                                         FALSE, getWindowExStyle(window),
//                                         LOWORD(wParam));

//                size->cx += (target.right - target.left) -
//                            (source.right - source.left);
//                size->cy += (target.bottom - target.top) -
//                            (source.bottom - source.top);
//                return TRUE;
//            }

//            break;
//        }

//        case WM_DPICHANGED:
//        {
//            const float xscale = HIWORD(wParam) / (float) USER_DEFAULT_SCREEN_DPI;
//            const float yscale = LOWORD(wParam) / (float) USER_DEFAULT_SCREEN_DPI;

//            // Resize windowed mode windows that either permit rescaling or that
//            // need it to compensate for non-client area scaling
//            if (!window->monitor &&
//                (((wManagerWin *)window->WindowData)->scaleToMonitor ||
//                 _wMInfoIsWindows10Version1703OrGreaterWin32()))
//            {
//                RECT* suggested = (RECT*) lParam;
//                SetWindowPos(((wManagerWin *)window->WindowData)->handle, HWND_TOP,
//                             suggested->left,
//                             suggested->top,
//                             suggested->right - suggested->left,
//                             suggested->bottom - suggested->top,
//                             SWP_NOACTIVATE | SWP_NOZORDER);
//            }

//            _wMInfoInputWindowContentScale(window, xscale, yscale);
//            break;
//        }

        case WM_SETCURSOR:
        {
            if (LOWORD(lParam) == HTCLIENT)
            {
                updateCursorImage(window);
                return TRUE;
            }

            break;
        }

        case WM_DROPFILES:
        {
            HDROP drop = (HDROP) wParam;
            POINT pt;
            int i;

            const int count = DragQueryFileW(drop, 0xffffffff, NULL, 0);
            char** paths = calloc(count, sizeof(char*));

            // Move the mouse to the position of the drop
            DragQueryPoint(drop, &pt);
            _wManagerInputCursorPos(window, pt.x, pt.y);

            for (i = 0;  i < count;  i++)
            {
                const UINT length = DragQueryFileW(drop, i, NULL, 0);
                WCHAR* buffer = calloc((size_t) length + 1, sizeof(WCHAR));

                DragQueryFileW(drop, i, buffer, length + 1);
                paths[i] = _wManagerCreateUTF8FromWideStringWin32(buffer);

                free(buffer);
            }

            _wManagerInputDrop(window, count, (const char**) paths);

            for (i = 0;  i < count;  i++)
                free(paths[i]);
            free(paths);

            DragFinish(drop);
            return 0;
        }
    }

    return DefWindowProcW(hWnd, uMsg, wParam, lParam);
}

int createNativeWindow(wManagerWindow* window,
                              const _wManagerwndconfig* wndconfig,
                              const _wManagerfbconfig* fbconfig)
{
    int frameX, frameY, frameWidth, frameHeight;

    ((wManagerWin *)_wMWindow.WindowData)->handle = GetModuleHandleW(0);

    DWORD style = getWindowStyle(window);
    DWORD exStyle = getWindowExStyle(window);

    WNDCLASSW   wc;
    memset(&wc, 0, sizeof(WNDCLASSW));

    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WindowProc;
    wc.hInstance     = ((wManagerWin *)_wMWindow.WindowData)->instance;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = L"ZEngine";
    wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);

    WCHAR*  wideTitle = _wManagerCreateWideStringFromUTF8Win32(wndconfig->title);
    if (!wideTitle)
            return false;

    ((wManagerWin *)_wMWindow.WindowData)->mainWindowClass = RegisterClassW(&wc);

    ((wManagerWin *)window->WindowData)->handle = CreateWindowExW(exStyle,
                                     MAKEINTATOM(((wManagerWin *)_wMWindow.WindowData)->mainWindowClass),
                                     wideTitle,
                                     style,
                                     100, 100,
                                     wndconfig->width, wndconfig->height,
                                     NULL,
                                     NULL,
                                     ((wManagerWin *)_wMWindow.WindowData)->instance,
                                     NULL);

    free(wideTitle);

    if (window->monitor)
       {
           MONITORINFO mi = { sizeof(mi) };
           GetMonitorInfoW(((_wManagerMonitorWin32 *)window->monitor->MonitorData)->handle, &mi);

           // NOTE: This window placement is temporary and approximate, as the
           //       correct position and size cannot be known until the monitor
           //       video mode has been picked in _glfwSetVideoModeWin32
           frameX = mi.rcMonitor.left;
           frameY = mi.rcMonitor.top;
           frameWidth  = mi.rcMonitor.right - mi.rcMonitor.left;
           frameHeight = mi.rcMonitor.bottom - mi.rcMonitor.top;
       }
       else
       {
           RECT rect = { 0, 0, wndconfig->width, wndconfig->height };

           window->hints.window.maximized = wndconfig->maximized;
           if (wndconfig->maximized)
               style |= WS_MAXIMIZE;

           AdjustWindowRectEx(&rect, style, FALSE, exStyle);

           if (wndconfig->xpos == ENGINE_ANY_POSITION && wndconfig->ypos == ENGINE_ANY_POSITION)
           {
               frameX = CW_USEDEFAULT;
               frameY = CW_USEDEFAULT;
           }
           else
           {
               frameX = wndconfig->xpos + rect.left;
               frameY = wndconfig->ypos + rect.top;
           }

           frameWidth  = rect.right - rect.left;
           frameHeight = rect.bottom - rect.top;
       }

       wideTitle = _wManagerCreateWideStringFromUTF8Win32(wndconfig->title);
       if (!wideTitle)
           return false;

       ((wManagerWin *)window->WindowData)->handle = CreateWindowExW(exStyle,
                                              MAKEINTATOM(((wManagerWin *)_wMWindow.WindowData)->mainWindowClass),
                                              wideTitle,
                                              style,
                                              frameX, frameY,
                                              frameWidth, frameHeight,
                                              NULL, // No parent window
                                              NULL, // No window menu
                                              ((wManagerWin *)_wMWindow.WindowData)->instance,
                                              (LPVOID) wndconfig);

       free(wideTitle);

       if (!((wManagerWin *)window->WindowData)->handle)
       {
           printf("Win32: Failed to create window\n");
           return false;
       }

       SetPropW(((wManagerWin *)window->WindowData)->handle, L"ZEngine", window);

       window->scaleToMonitor = wndconfig->scaleToMonitor;
       window->keymenu = wndconfig->win32.keymenu;
       window->showDefault = wndconfig->win32.showDefault;

       if (!window->monitor)
       {
           RECT rect = { 0, 0, wndconfig->width, wndconfig->height };
           WINDOWPLACEMENT wp = { sizeof(wp) };
           const HMONITOR mh = MonitorFromWindow(((wManagerWin *)window->WindowData)->handle,
                                                 MONITOR_DEFAULTTONEAREST);

           // Adjust window rect to account for DPI scaling of the window frame and
           // (if enabled) DPI scaling of the content area
           // This cannot be done until we know what monitor the window was placed on
           // Only update the restored window rect as the window may be maximized

           /*if (wndconfig->scaleToMonitor)
           {
               float xscale, yscale;
               _wManagerGetHMONITORContentScaleWin32(mh, &xscale, &yscale);

               if (xscale > 0.f && yscale > 0.f)
               {
                   rect.right = (int) (rect.right * xscale);
                   rect.bottom = (int) (rect.bottom * yscale);
               }
           }*/

           AdjustWindowRectEx(&rect, style, FALSE, exStyle);

           GetWindowPlacement(((wManagerWin *)window->WindowData)->handle, &wp);
           OffsetRect(&rect,
                      wp.rcNormalPosition.left - rect.left,
                      wp.rcNormalPosition.top - rect.top);

           wp.rcNormalPosition = rect;
           wp.showCmd = SW_HIDE;
           SetWindowPlacement(((wManagerWin *)window->WindowData)->handle, &wp);

           // Adjust rect of maximized undecorated window, because by default Windows will
           // make such a window cover the whole monitor instead of its workarea

           if (wndconfig->maximized && !wndconfig->decorated)
           {
               MONITORINFO mi = { sizeof(mi) };
               GetMonitorInfoW(mh, &mi);

               SetWindowPos(((wManagerWin *)window->WindowData)->handle, HWND_TOP,
                            mi.rcWork.left,
                            mi.rcWork.top,
                            mi.rcWork.right - mi.rcWork.left,
                            mi.rcWork.bottom - mi.rcWork.top,
                            SWP_NOACTIVATE | SWP_NOZORDER);
           }
       }

       DragAcceptFiles(((wManagerWin *)window->WindowData)->handle, TRUE);

       if (fbconfig->transparent)
       {
           updateFramebufferTransparency(window);
           window->transparent = true;
       }

       _wManagerGetWindowSizeWin32(window, &window->width, &window->height);

       return true;

    return true;
}

void fitToMonitor(wManagerWindow* window)
{
    MONITORINFO mi = { sizeof(mi) };
    GetMonitorInfoW(((wManagerWin *)window->monitor->MonitorData)->handle, &mi);
    SetWindowPos(((wManagerWin *)window->monitor->MonitorData)->handle, HWND_TOPMOST,
                 mi.rcMonitor.left,
                 mi.rcMonitor.top,
                 mi.rcMonitor.right - mi.rcMonitor.left,
                 mi.rcMonitor.bottom - mi.rcMonitor.top,
                 SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOCOPYBITS);
}

// Make the specified window and its video mode active on its monitor
//
void acquireMonitor(wManagerWindow* window)
{
    /*if (!((wManagerWin *)_wMWindow.WindowData)->acquiredMonitorCount)
    {
        SetThreadExecutionState(ES_CONTINUOUS | ES_DISPLAY_REQUIRED);

        // HACK: When mouse trails are enabled the cursor becomes invisible when
        //       the OpenGL ICD switches to page flipping
        SystemParametersInfoW(SPI_GETMOUSETRAILS, 0, &((wManagerWin *)_wMWindow.WindowData)->mouseTrailSize, 0);
        SystemParametersInfoW(SPI_SETMOUSETRAILS, 0, 0, 0);
    }

    if (!window->monitor->window)
        ((wManagerWin *)_wMWindow.WindowData)->acquiredMonitorCount++;

    _wManagerSetVideoModeWin32(window->monitor, &window->videoMode);
    _wManagerInputMonitorWindow(window->monitor, window);*/
}

// Remove the window and restore the original video mode
//
void releaseMonitor(wManagerWindow* window)
{
    /*if (window->monitor->window != window)
        return;

    ((wManagerWin *)_wMWindow.WindowData)->acquiredMonitorCount--;
    if (!((wManagerWin *)_wMWindow.WindowData)->acquiredMonitorCount)
    {
        SetThreadExecutionState(ES_CONTINUOUS);

        // HACK: Restore mouse trail length saved in acquireMonitor
        SystemParametersInfoW(SPI_SETMOUSETRAILS, ((wManagerWin *)_wMWindow.WindowData)->mouseTrailSize, 0, 0);
    }

    _wManagerInputMonitorWindow(window->monitor, NULL);
    _wManagerRestoreVideoModeWin32(window->monitor);*/
}

int _wManagerCreateWindowWin32(wManagerWindow *window,
                               const _wManagerwndconfig* wndconfig,
                               const _wManagerfbconfig* fbconfig)
{

    if (!createNativeWindow(window, wndconfig, fbconfig))
            return false;

    /*if (window->monitor)
    {
        _wManagerShowWindowWin32(window);
        _wManagerFocusWindowWin32(window);
        acquireMonitor(window);
        fitToMonitor(window);

        if (wndconfig->centerCursor)
            _wManagerCenterCursorInContentArea(window);
    }
    else*/
    {
        if (wndconfig->visible)
        {
            _wManagerShowWindowWin32(window);
            if (wndconfig->focused)
                _wManagerFocusWindowWin32(window);
        }
    }

    return true;
}

void _wManagerDestroyWindowWin32(wManagerWindow* window)
{
    if (window->monitor)
        releaseMonitor(window);

    if (window->context.destroy)
        window->context.destroy(window);

    if (((wManagerWin* )_wMWindow.WindowData)->disabledCursorWindow == window)
        enableCursor(window);

    if (((wManagerWin* )_wMWindow.WindowData)->capturedCursorWindow == window)
        releaseCursor(window);

    if (((wManagerWin *)window->WindowData)->handle)
    {
        RemovePropW(((wManagerWin *)window->WindowData)->handle, L"ZEngine");
        DestroyWindow(((wManagerWin *)window->WindowData)->handle);
        ((wManagerWin *)window->WindowData)->handle = NULL;
    }

    /*if (((wManagerWin *)window->WindowData)->bigIcon)
        DestroyIcon(((wManagerWin *)window->WindowData)->bigIcon);

    if (((wManagerWin *)window->WindowData)->smallIcon)
        DestroyIcon(((wManagerWin *)window->WindowData)->smallIcon);*/

}

void _wManagerSetWindowTitleWin32(wManagerWindow* window, const char* title)
{
    WCHAR* wideTitle = _wManagerCreateWideStringFromUTF8Win32(title);
    if (!wideTitle)
        return;

    SetWindowTextW(((wManagerWin *)window->WindowData)->handle, wideTitle);
    free(wideTitle);
}

void _wManagerSetWindowIconWin32(wManagerWindow* window, int count, void* images)
{
    //HICON bigIcon = NULL, smallIcon = NULL;

    /*if (count)
    {
        const GLFWimage* bigImage = chooseImage(count, images,
                                                GetSystemMetrics(SM_CXICON),
                                                GetSystemMetrics(SM_CYICON));
        const GLFWimage* smallImage = chooseImage(count, images,
                                                  GetSystemMetrics(SM_CXSMICON),
                                                  GetSystemMetrics(SM_CYSMICON));

        bigIcon = createIcon(bigImage, 0, 0, ENGINE_TRUE);
        smallIcon = createIcon(smallImage, 0, 0, ENGINE_TRUE);
    }
    else
    {
        bigIcon = (HICON) GetClassLongPtrW(((wManagerWin *)window->WindowData)->handle, GCLP_HICON);
        smallIcon = (HICON) GetClassLongPtrW(((wManagerWin *)window->WindowData)->handle, GCLP_HICONSM);
    }

    SendMessageW(((wManagerWin *)window->WindowData)->handle, WM_SETICON, ICON_BIG, (LPARAM) bigIcon);
    SendMessageW(((wManagerWin *)window->WindowData)->handle, WM_SETICON, ICON_SMALL, (LPARAM) smallIcon);

    if (((wManagerWin *)window->WindowData)->bigIcon)
        DestroyIcon(((wManagerWin *)window->WindowData)->bigIcon);

    if (((wManagerWin *)window->WindowData)->smallIcon)
        DestroyIcon(((wManagerWin *)window->WindowData)->smallIcon);

    if (count)
    {
        ((wManagerWin *)window->WindowData)->bigIcon = bigIcon;
        ((wManagerWin *)window->WindowData)->smallIcon = smallIcon;
    }*/
}

void _wManagerGetWindowPosWin32(wManagerWindow* window, int* xpos, int* ypos)
{
    POINT pos = { 0, 0 };
    ClientToScreen(((wManagerWin *)window->WindowData)->handle, &pos);

    if (xpos)
        *xpos = pos.x;
    if (ypos)
        *ypos = pos.y;
}

void _wManagerSetWindowPosWin32(wManagerWindow* window, int xpos, int ypos)
{
    RECT rect = { xpos, ypos, xpos, ypos };

    AdjustWindowRectEx(&rect, getWindowStyle(window),
                       FALSE, getWindowExStyle(window));

    SetWindowPos(((wManagerWin *)window->WindowData)->handle, NULL, rect.left, rect.top, 0, 0,
                 SWP_NOACTIVATE | SWP_NOZORDER | SWP_NOSIZE);
}

void _wManagerGetRequiredInstanceExtensionsWin32(char** extensions)
{
    if (!_wMInfo.vk.KHR_surface || !_wMInfo.vk.KHR_win32_surface)
        return;

    extensions[0] = "VK_KHR_surface";
    extensions[1] = "VK_KHR_win32_surface";
}

extern const char* _wManagerGetVulkanResultString(VkResult result);

int32_t _wManagerGetPhysicalDevicePresentationSupportWin32(VkInstance instance,
                                                        VkPhysicalDevice device,
                                                        uint32_t queuefamily)
{
    PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR
        vkGetPhysicalDeviceWin32PresentationSupportKHR =
        (PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR)
        vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceWin32PresentationSupportKHR");
    if (!vkGetPhysicalDeviceWin32PresentationSupportKHR)
    {
        printf("Win32: Vulkan instance missing VK_KHR_win32_surface extension\n");
        return false;
    }

    return vkGetPhysicalDeviceWin32PresentationSupportKHR(device, queuefamily);
}


VkResult _wManagerCreateWindowSurfaceWin32(VkInstance instance,
                                       wManagerWindow* window,
                                       const VkAllocationCallbacks* allocator,
                                       VkSurfaceKHR* surface){
    VkResult err;
    VkWin32SurfaceCreateInfoKHR sci;
    PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR;

    vkCreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(instance, "vkCreateWin32SurfaceKHR");
    if (!vkCreateWin32SurfaceKHR)
    {
        printf("Win32: Vulkan instance missing VK_KHR_win32_surface extension\n");
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }

    memset(&sci, 0, sizeof(sci));
    sci.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    sci.hinstance = ((wManagerWin *)_wMWindow.WindowData)->instance;
    sci.hwnd = ((wManagerWin *)window->WindowData)->handle;

    err = vkCreateWin32SurfaceKHR(instance, &sci, allocator, surface);
    if (err)
    {
        printf("Win32: Failed to create Vulkan surface: %s\n", _wManagerGetVulkanResultString(err));
    }

    return err;
}

void _wManagerPollEventsWin32(){
    MSG msg;
    HWND handle;
    wManagerWindow *window;

    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
        {
            // NOTE: While GLFW does not itself post WM_QUIT, other processes
            //       may post it to this one, for example Task Manager
            // HACK: Treat WM_QUIT as a close on all windows

            window = e_window;//_wMInfo.windowListHead;
            while (window)
            {
                _wManagerInputWindowCloseRequest(window);
                window = window->next;
            }
        }
        else
        {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
    }

    // HACK: Release modifier keys that the system did not emit KEYUP for
    // NOTE: Shift keys on Windows tend to "stick" when both are pressed as
    //       no key up message is generated by the first key release
    // NOTE: Windows key is not reported as released by the Win+V hotkey
    //       Other Win hotkeys are handled implicitly by _wMInfoInputWindowFocus
    //       because they change the input focus
    // NOTE: The other half of this is in the WM_*KEY* handler in windowProc
    handle = GetActiveWindow();
    if (handle)
    {
        window = GetPropW(handle, L"ZEngine");
        if (window)
        {
            int i;
            const int keys[4][2] =
            {
                { VK_LSHIFT, ENGINE_KEY_LEFT_SHIFT },
                { VK_RSHIFT, ENGINE_KEY_RIGHT_SHIFT },
                { VK_LWIN, ENGINE_KEY_LEFT_SUPER },
                { VK_RWIN, ENGINE_KEY_RIGHT_SUPER }
            };

            for (i = 0;  i < 4;  i++)
            {
                const int vk = keys[i][0];
                const int key = keys[i][1];
                const int scancode = ((wManagerWin* )window->WindowData)->scancodes[key];

                if ((GetKeyState(vk) & 0x8000))
                    continue;
                if (window->keys[key] != ENGINE_PRESS)
                    continue;

                _wManagerInputKey(window, key, scancode, ENGINE_RELEASE, getKeyMods());
            }
        }
    }

    window = ((wManagerWin *)_wMWindow.WindowData)->disabledCursorWindow;
    if (window)
    {
        int width, height;
        _wManagerGetWindowSizeWin32(window, &width, &height);

        // NOTE: Re-center the cursor only if it has moved since the last call,
        //       to avoid breaking glfwWaitEvents with WM_MOUSEMOVE
        // The re-center is required in order to prevent the mouse cursor stopping at the edges of the screen.
        if (((wManagerWin* )window->WindowData)->lastCursorPosX != width / 2 ||
            ((wManagerWin* )window->WindowData)->lastCursorPosY != height / 2)
        {
            _wManagerSetCursorPosWin32(window, width / 2, height / 2);
        }
    }
}

void _wManagerWaitEventsWin32(void)
{
    WaitMessage();

    _wManagerPollEventsWin32();
}

void _wManagerWaitEventsTimeoutWin32(double timeout)
{
    MsgWaitForMultipleObjects(0, NULL, FALSE, (DWORD) (timeout * 1e3), QS_ALLINPUT);

    _wManagerPollEventsWin32();
}

void _wManagerPostEmptyEventWin32(void)
{
    //PostMessageW(((wManagerWin* )_wMWindow.WindowData)->helperWindowHandle, WM_NULL, 0, 0);
}

extern void _wManagerInputWindowMonitor(wManagerWindow* window, _wManagerMonitor* monitor);

void _wManagerSetWindowMonitorWin32(wManagerWindow* window,
                                _wManagerMonitor* monitor,
                                int xpos, int ypos,
                                int width, int height,
                                int refreshRate)
{
    /*if (window->monitor == monitor)
    {
        if (monitor)
        {
            if (monitor->window == window)
            {
                acquireMonitor(window);
                fitToMonitor(window);
            }
        }
        else
        {
            RECT rect = { xpos, ypos, xpos + width, ypos + height };


            AdjustWindowRectEx(&rect, getWindowStyle(window),
                               FALSE, getWindowExStyle(window));

            SetWindowPos(((wManagerWin *)window->WindowData)->handle, HWND_TOP,
                         rect.left, rect.top,
                         rect.right - rect.left, rect.bottom - rect.top,
                         SWP_NOCOPYBITS | SWP_NOACTIVATE | SWP_NOZORDER);
        }

        return;
    }

    if (window->monitor)
        releaseMonitor(window);

    _wManagerInputWindowMonitor(window, monitor);

    if (window->monitor)
    {
        MONITORINFO mi = { sizeof(mi) };
        UINT flags = SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOCOPYBITS;

        if (window->decorated)
        {
            DWORD style = GetWindowLongW(((wManagerWin *)window->WindowData)->handle, GWL_STYLE);
            style &= ~WS_OVERLAPPEDWINDOW;
            style |= getWindowStyle(window);
            SetWindowLongW(((wManagerWin *)window->WindowData)->handle, GWL_STYLE, style);
            flags |= SWP_FRAMECHANGED;
        }

        acquireMonitor(window);

        GetMonitorInfoW(((_wManagerMonitorWin32 *)window->monitor->MonitorData)->handle, &mi);
        SetWindowPos(((wManagerWin *)window->WindowData)->handle, HWND_TOPMOST,
                     mi.rcMonitor.left,
                     mi.rcMonitor.top,
                     mi.rcMonitor.right - mi.rcMonitor.left,
                     mi.rcMonitor.bottom - mi.rcMonitor.top,
                     flags);
    }
    else
    {
        HWND after;
        RECT rect = { xpos, ypos, xpos + width, ypos + height };
        DWORD style = GetWindowLongW(((wManagerWin *)window->WindowData)->handle, GWL_STYLE);
        UINT flags = SWP_NOACTIVATE | SWP_NOCOPYBITS;

        if (window->decorated)
        {
            style &= ~WS_POPUP;
            style |= getWindowStyle(window);
            SetWindowLongW(((wManagerWin *)window->WindowData)->handle, GWL_STYLE, style);

            flags |= SWP_FRAMECHANGED;
        }

        if (window->floating)
            after = HWND_TOPMOST;
        else
            after = HWND_NOTOPMOST;


        AdjustWindowRectEx(&rect, getWindowStyle(window),
                           FALSE, getWindowExStyle(window));

        SetWindowPos(((wManagerWin *)window->WindowData)->handle, after,
                     rect.left, rect.top,
                     rect.right - rect.left, rect.bottom - rect.top,
                     flags);
    }*/
}

#endif
