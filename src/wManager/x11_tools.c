#include "wManager/window_manager.h"
#include "wManager/x11_defines.h"


#ifdef _X11_

#include <unistd.h>
#include <sys/time.h>

#define CLOCK_REALTIME 0

extern wManagerWindow _wMWindow;

extern unsigned long _wManagerGetWindowPropertyX11(Window window,
                                                   Atom property,
                                                   Atom type,
                                                   unsigned char** value);

void _wManagerPlatformInitTimer(void)
{
    _wMWindow.time_clock = CLOCK_REALTIME;
    _wMWindow.time_freq = 1000000000;

/*#if defined(_POSIX_MONOTONIC_CLOCK)
    struct timespec ts;
    if (clock_gettime(CLOCK_MONOTONIC, &ts) == 0)
        _glfw.timer.posix.clock = CLOCK_MONOTONIC;
#endif*/
}

uint64_t _wManagerPlatformGetTimerValue(void)
{
    struct timespec ts;
    clock_gettime(_wMWindow.time_clock, &ts);
    return (uint64_t) ts.tv_sec * _wMWindow.time_freq + (uint64_t) ts.tv_nsec;
}

uint64_t _wManagerPlatformGetTimerFrequency(void)
{
    return _wMWindow.time_freq;
}


// Enable XI2 raw mouse motion events
//
void enableRawMouseMotion(wManagerWindow* window)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;

    XIEventMask em;
    unsigned char mask[XIMaskLen(XI_RawMotion)] = { 0 };

    em.deviceid = XIAllMasterDevices;
    em.mask_len = sizeof(mask);
    em.mask = mask;
    XISetMask(mask, XI_RawMotion);

    XISelectEvents(wX11->display, wX11->root, &em, 1);
}

// Disable XI2 raw mouse motion events
//
void disableRawMouseMotion(wManagerWindow* window)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;

    XIEventMask em;
    unsigned char mask[] = { 0 };

    em.deviceid = XIAllMasterDevices;
    em.mask_len = sizeof(mask);
    em.mask = mask;

    XISelectEvents(wX11->display, wX11->root, &em, 1);
}

// Updates the cursor image according to its cursor mode
//
void updateCursorImage(wManagerWindow* window)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;
    wManagerX11 *x11 = window->WindowData;

    if (window->cursorMode == ENGINE_CURSOR_NORMAL ||
        window->cursorMode == ENGINE_CURSOR_CAPTURED)
    {
        /*if (window->cursor)
        {
            XDefineCursor(wX11->display, x11->handle,
                          window->cursor->x11.handle);
        }
        else*/
            XUndefineCursor(wX11->display, x11->handle);
    }
    else
    {
        XDefineCursor(wX11->display, x11->handle,
                      wX11->hiddenCursorHandle);
    }
}

// Sets the cursor clip rect to the window content area
//
void captureCursor(wManagerWindow *window)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;
    wManagerX11 *x11 = window->WindowData;

    XGrabPointer(wX11->display, x11->handle, True,
                    ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
                    GrabModeAsync, GrabModeAsync,
                    x11->handle,
                    None,
                    CurrentTime);
}

// Disabled clip cursor
//
void releaseCursor(wManagerWindow *window)
{
    wManagerX11 *wX11 = _wMWindow.WindowData;

    XUngrabPointer(wX11->display, CurrentTime);
}

// Apply disabled cursor mode to a focused window
//
void disableCursor(wManagerWindow *window)
{
    ((wManagerX11 *)window->WindowData)->disabledCursorWindow = window;
    window->platform.getCursorPos(window, &((wManagerX11 *)window->WindowData)->restoreCursorPosX,
                                &((wManagerX11 *)window->WindowData)->restoreCursorPosY);
    updateCursorImage(window);

    int width, height;

    window->platform.getWindowSize(window, &width, &height);
    window->platform.setCursorPos(window, width / 2.0f, height / 2.0f);

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

    ((wManagerX11 *)window->WindowData)->disabledCursorWindow = NULL;
    releaseCursor(window);
    window->platform.setCursorPos(window, ((wManagerX11 *)window->WindowData)->restoreCursorPosX,
                               ((wManagerX11 *)window->WindowData)->restoreCursorPosY);
    updateCursorImage(window);
}

// Returns whether the window is iconified
//
int getWindowState(wManagerWindow* window)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;
    wManagerX11 *x11 = window->WindowData;

    int result = WithdrawnState;
    struct {
        CARD32 state;
        Window icon;
    } *state = NULL;

    if (_wManagerGetWindowPropertyX11(x11->handle,
                                  wX11->WM_STATE,
                                  wX11->WM_STATE,
                                  (unsigned char**) &state) >= 2)
    {
        result = state->state;
    }

    if (state)
        XFree(state);

    return result;
}

#endif
