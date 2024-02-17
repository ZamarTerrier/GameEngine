#include "wManager/manager_includes.h"
#include "wManager/x11_defines.h"
#include "wManager/window_manager.h"
#include "wManager/input_manager.h"


#ifdef _X11_

// Action for EWMH client messages
#define _NET_WM_STATE_REMOVE        0
#define _NET_WM_STATE_ADD           1
#define _NET_WM_STATE_TOGGLE        2

// Additional mouse button names for XButtonEvent
#define Button6            6
#define Button7            7

#define MWM_HINTS_DECORATIONS   2
#define MWM_DECOR_ALL           1

#define _ENGINE_XDND_VERSION 5

#define ENGINE_INVALID_CODEPOINT 0xffffffffu

extern wManagerWindow _wMWindow;
extern wManagerInfo _wMInfo;

extern void enableRawMouseMotion(wManagerWindow* window);
extern void disableRawMouseMotion(wManagerWindow* window);

extern void enableCursor(wManagerWindow *window);
extern void disableCursor(wManagerWindow *window);

extern void updateCursorImage(wManagerWindow* window);

extern int getWindowState(wManagerWindow* window);

unsigned long _wManagerGetWindowPropertyX11(Window window,
                                        Atom property,
                                        Atom type,
                                        unsigned char** value)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;

    Atom actualType;
    int actualFormat;
    unsigned long itemCount, bytesAfter;

    XGetWindowProperty(wX11->display,
                       window,
                       property,
                       0,
                       LONG_MAX,
                       False,
                       type,
                       &actualType,
                       &actualFormat,
                       &itemCount,
                       &bytesAfter,
                       value);

    return itemCount;
}

void _wManagerGetCursorPosX11(wManagerWindow* window, double* xpos, double* ypos)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;
    wManagerX11 *x11 = window->WindowData;

    Window root, child;
    int rootX, rootY, childX, childY;
    unsigned int mask;

    XQueryPointer(wX11->display, x11->handle,
                  &root, &child,
                  &rootX, &rootY, &childX, &childY,
                  &mask);

    if (xpos)
        *xpos = childX;
    if (ypos)
        *ypos = childY;
}

void _wManagerSetCursorPosX11(wManagerWindow* window, double x, double y)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;
    wManagerX11 *x11 = window->WindowData;

    // Store the new position so it can be recognized later
    x11->warpCursorPosX = (int) x;
    x11->warpCursorPosY = (int) y;

    XWarpPointer(wX11->display, None, x11->handle,
                 0,0,0,0, (int) x, (int) y);
    XFlush(wX11->display);
}


uint32_t _wManagerWindowFocusedX11(wManagerWindow* window)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;
    wManagerX11 *x11 = window->WindowData;

    Window focused;
    int state;

    XGetInputFocus(wX11->display, &focused, &state);
    return x11->handle == focused;
}

void _wManagerSetCursorModeX11(wManagerWindow* window, int mode)
{
    wManagerX11 *wX11 = _wMWindow.WindowData;

    if (_wManagerWindowFocusedX11(window))
    {
        if (mode == ENGINE_CURSOR_DISABLED)
        {
            window->platform.getCursorPos(window,
                                 &wX11->restoreCursorPosX,
                                 &wX11->restoreCursorPosY);
            //_wManagerCenterCursorInContentArea(window);
            if (window->rawMouseMotion)
                enableRawMouseMotion(window);
        }
        else if (wX11->disabledCursorWindow == window)
        {
            if (window->rawMouseMotion)
                disableRawMouseMotion(window);
        }

        if (mode == ENGINE_CURSOR_DISABLED || mode == ENGINE_CURSOR_CAPTURED)
            captureCursor(window);
        else
            releaseCursor();

        if (mode == ENGINE_CURSOR_DISABLED)
            wX11->disabledCursorWindow = window;
        else if (wX11->disabledCursorWindow == window)
        {
            wX11->disabledCursorWindow = NULL;
            _wManagerSetCursorPosX11(window,
                                 wX11->restoreCursorPosX,
                                 wX11->restoreCursorPosY);
        }
    }

    updateCursorImage(window);
    XFlush(wX11->display);
}

void _wManagerSetRawMouseMotionX11(wManagerWindow *window, uint32_t enabled)
{
    wManagerX11 *wX11 = _wMWindow.WindowData;

    if (!wX11->xi.available)
        return;

    if (wX11->disabledCursorWindow != window)
        return;

    if (enabled)
        enableRawMouseMotion(window);
    else
        disableRawMouseMotion(window);
}

uint32_t _wManagerRawMouseMotionSupportedX11(void)
{
    wManagerX11 *wX11 = _wMWindow.WindowData;

    return wX11->xi.available;
}

size_t _wManagerEncodeUTF8(char* s, uint32_t codepoint)
{
    size_t count = 0;

    if (codepoint < 0x80)
        s[count++] = (char) codepoint;
    else if (codepoint < 0x800)
    {
        s[count++] = (codepoint >> 6) | 0xc0;
        s[count++] = (codepoint & 0x3f) | 0x80;
    }
    else if (codepoint < 0x10000)
    {
        s[count++] = (codepoint >> 12) | 0xe0;
        s[count++] = ((codepoint >> 6) & 0x3f) | 0x80;
        s[count++] = (codepoint & 0x3f) | 0x80;
    }
    else if (codepoint < 0x110000)
    {
        s[count++] = (codepoint >> 18) | 0xf0;
        s[count++] = ((codepoint >> 12) & 0x3f) | 0x80;
        s[count++] = ((codepoint >> 6) & 0x3f) | 0x80;
        s[count++] = (codepoint & 0x3f) | 0x80;
    }

    return count;
}

uint32_t _wManagerKeySym2Unicode(unsigned int keysym)
{
    int min = 0;
    int max = sizeof(keysymtab) / sizeof(struct codepair) - 1;
    int mid;

    // First check for Latin-1 characters (1:1 mapping)
    if ((keysym >= 0x0020 && keysym <= 0x007e) ||
        (keysym >= 0x00a0 && keysym <= 0x00ff))
    {
        return keysym;
    }

    // Also check for directly encoded 24-bit UCS characters
    if ((keysym & 0xff000000) == 0x01000000)
        return keysym & 0x00ffffff;

    // Binary search in table
    while (max >= min)
    {
        mid = (min + max) / 2;
        if (keysymtab[mid].keysym < keysym)
            min = mid + 1;
        else if (keysymtab[mid].keysym > keysym)
            max = mid - 1;
        else
            return keysymtab[mid].ucs;
    }

    // No matching Unicode value found
    return ENGINE_INVALID_CODEPOINT;
}

int _wManagerGetKeyScancodeX11(int key)
{
    wManagerX11 *wX11 = _wMWindow.WindowData;

    return wX11->scancodes[key];
}
const char* _wManagerGetScancodeNameX11(int scancode)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;

    if (!wX11->xkb.available)
        return NULL;

    if (scancode < 0 || (scancode > 0xff))
    {
        printf("Invalid scancode %i\n", scancode);
        return NULL;
    }

    const int key = wX11->keycodes[scancode];
    if (key == ENGINE_KEY_UNKNOWN)
        return NULL;

    const KeySym keysym = XkbKeycodeToKeysym(wX11->display,
                                             scancode, wX11->xkb.group, 0);
    if (keysym == NoSymbol)
        return NULL;

    const uint32_t codepoint = _wManagerKeySym2Unicode(keysym);
    if (codepoint == ENGINE_INVALID_CODEPOINT)
        return NULL;

    const size_t count = _wManagerEncodeUTF8(wX11->keynames[key], codepoint);
    if (count == 0)
        return NULL;

    wX11->keynames[key][count] = '\0';
    return wX11->keynames[key];
}

// Clear its handle when the input context has been destroyed
//
void inputContextDestroyCallback(XIC ic, XPointer clientData, XPointer callData)
{

    wManagerX11 *x11 = _wMWindow.WindowData;

    wManagerWindow* window = (wManagerWindow*) clientData;
    x11->ic = NULL;
}

// X error handler
//
int errorHandler(Display *display, XErrorEvent* event)
{
    wManagerX11 *x11 = _wMWindow.WindowData;

    if (x11->display != display)
        return 0;

    x11->errorCode = event->error_code;
    return 0;
}

void _wManagerGrabErrorHandlerX11(void)
{
    wManagerX11 *x11 = _wMWindow.WindowData;

    x11->errorCode = Success;
    x11->errorHandler = XSetErrorHandler(errorHandler);
}

// Clears the X error handler callback
//
void _wManagerReleaseErrorHandlerX11(void)
{
    wManagerX11 *x11 = _wMWindow.WindowData;

    // Synchronize to make sure all commands are processed
    XSync(x11->display, False);
    XSetErrorHandler(x11->errorHandler);
    x11->errorHandler = NULL;
}

uint32_t _wManagerIsVisualTransparentX11(Visual* visual)
{
    wManagerX11 *x11 = _wMWindow.WindowData;

    if (!x11->xrender.available)
        return false;

    XRenderPictFormat* pf = XRenderFindVisualFormat(x11->display, visual);
    return pf && pf->direct.alphaMask;
}

void _wManagerSetWindowDecoratedX11(wManagerWindow* window, uint32_t enabled)
{
    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    struct
    {
        unsigned long flags;
        unsigned long functions;
        unsigned long decorations;
        long input_mode;
        unsigned long status;
    } hints = {0};

    hints.flags = MWM_HINTS_DECORATIONS;
    hints.decorations = enabled ? MWM_DECOR_ALL : 0;

    XChangeProperty(wX11->display, x11->handle,
                    wX11->MOTIF_WM_HINTS,
                    wX11->MOTIF_WM_HINTS, 32,
                    PropModeReplace,
                    (unsigned char*) &hints,
                    sizeof(hints) / sizeof(long));
}

void _wManagerCreateInputContextX11(wManagerWindow* window)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    XIMCallback callback;
    callback.callback = (XIMProc) inputContextDestroyCallback;
    callback.client_data = (XPointer) window;

    x11->ic = XCreateIC(wX11->im,
                               XNInputStyle,
                               XIMPreeditNothing | XIMStatusNothing,
                               XNClientWindow,
                               x11->handle,
                               XNFocusWindow,
                               x11->handle,
                               XNDestroyCallback,
                               &callback,
                               NULL);

    if (x11->ic)
    {
        XWindowAttributes attribs;
        XGetWindowAttributes(wX11->display, x11->handle, &attribs);

        unsigned long filter = 0;
        if (XGetICValues(x11->ic, XNFilterEvents, &filter, NULL) == NULL)
        {
            XSelectInput(wX11->display,
                         x11->handle,
                         attribs.your_event_mask | filter);
        }
    }
}

void _wManagerSetWindowTitleX11(wManagerWindow* window, const char* title)
{
    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    if (wX11->xlib.utf8)
    {
        Xutf8SetWMProperties(wX11->display,
                             x11->handle,
                             title, title,
                             NULL, 0,
                             NULL, NULL, NULL);
    }

    XChangeProperty(wX11->display,  x11->handle,
                    wX11->NET_WM_NAME, wX11->UTF8_STRING, 8,
                    PropModeReplace,
                    (unsigned char*) title, strlen(title));

    XChangeProperty(wX11->display,  x11->handle,
                    wX11->NET_WM_ICON_NAME, wX11->UTF8_STRING, 8,
                    PropModeReplace,
                    (unsigned char*) title, strlen(title));

    XFlush(wX11->display);
}

void _wManagerGetWindowPosX11(wManagerWindow* window, int* xpos, int* ypos)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    Window dummy;
    int x, y;

    XTranslateCoordinates(wX11->display, x11->handle, wX11->root,
                          0, 0, &x, &y, &dummy);

    if (xpos)
        *xpos = x;
    if (ypos)
        *ypos = y;
}

void _wManagerSetWindowPosX11(wManagerWindow* window, int xpos, int ypos)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    // HACK: Explicitly setting PPosition to any value causes some WMs, notably
    //       Compiz and Metacity, to honor the position of unmapped windows
    if (!_wManagerWindowVisibleX11(window))
    {
        long supplied;
        XSizeHints* hints = XAllocSizeHints();

        if (XGetWMNormalHints(wX11->display, x11->handle, hints, &supplied))
        {
            hints->flags |= PPosition;
            hints->x = hints->y = 0;

            XSetWMNormalHints(wX11->display, x11->handle, hints);
        }

        XFree(hints);
    }

    XMoveWindow(wX11->display, x11->handle, xpos, ypos);
    XFlush(wX11->display);
}

void _wManagerGetWindowSizeX11(wManagerWindow* window, int* width, int* height)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    XWindowAttributes attribs;
    XGetWindowAttributes(wX11->display, x11->handle, &attribs);

    if (width)
        *width = attribs.width;
    if (height)
        *height = attribs.height;
}

void _wManagerSetWindowSizeX11(wManagerWindow* window, int width, int height)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    /*if (window->monitor)
    {
        if (window->monitor->window == window)
            acquireMonitor(window);
    }
    else
    {
        if (!window->resizable)
            updateNormalHints(window, width, height);

        XResizeWindow(wX11->display, x11->handle, width, height);
    }*/

    XFlush(wX11->display);
}

// Updates the normal hints according to the window settings
//
void updateNormalHints(wManagerWindow* window, int width, int height)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    XSizeHints* hints = XAllocSizeHints();

    long supplied;
    XGetWMNormalHints(wX11->display, x11->handle, hints, &supplied);

    hints->flags &= ~(PMinSize | PMaxSize | PAspect);

    /*if (!window->monitor)
    {
        if (window->resizable)
        {
            if (window->minwidth != ENGINE_DONT_CARE &&
                window->minheight != ENGINE_DONT_CARE)
            {
                hints->flags |= PMinSize;
                hints->min_width = window->minwidth;
                hints->min_height = window->minheight;
            }

            if (window->maxwidth != ENGINE_DONT_CARE &&
                window->maxheight != ENGINE_DONT_CARE)
            {
                hints->flags |= PMaxSize;
                hints->max_width = window->maxwidth;
                hints->max_height = window->maxheight;
            }

            if (window->numer != ENGINE_DONT_CARE &&
                window->denom != ENGINE_DONT_CARE)
            {
                hints->flags |= PAspect;
                hints->min_aspect.x = hints->max_aspect.x = window->numer;
                hints->min_aspect.y = hints->max_aspect.y = window->denom;
            }
        }
        else
        {
            hints->flags |= (PMinSize | PMaxSize);
            hints->min_width  = hints->max_width  = width;
            hints->min_height = hints->max_height = height;
        }
    }*/

    XSetWMNormalHints(wX11->display, x11->handle, hints);
    XFree(hints);
}

void _wManagerSetWindowSizeLimitsX11(wManagerWindow* window,
                                 int minwidth, int minheight,
                                 int maxwidth, int maxheight)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    int width, height;
    _wManagerGetWindowSizeX11(window, &width, &height);
    updateNormalHints(window, width, height);
    XFlush(wX11->display);
}

void _wManagerSetWindowAspectRatioX11(wManagerWindow* window, int numer, int denom)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;

    int width, height;
    _wManagerGetWindowSizeX11(window, &width, &height);
    updateNormalHints(window, width, height);
    XFlush(wX11->display);
}

void _wManagerGetFramebufferSizeX11(wManagerWindow* window, int* width, int* height)
{
    _wManagerGetWindowSizeX11(window, width, height);
}

// Returns whether it is a _NET_FRAME_EXTENTS event for the specified window
//
Bool isFrameExtentsEvent(Display* display, XEvent* event, XPointer pointer)
{
    wManagerWindow* window = (wManagerWindow*) pointer;

    wManagerX11 *wX11 = _wMWindow.WindowData;

    return event->type == PropertyNotify &&
           event->xproperty.state == PropertyNewValue &&
           event->xproperty.window == ((wManagerX11 *)window->WindowData)->handle &&
           event->xproperty.atom == wX11->NET_FRAME_EXTENTS;
}

extern int32_t _wManagerPollPOSIX(struct pollfd* fds, nfds_t count, double* timeout);

int32_t waitForX11Event(double* timeout)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;

    struct pollfd fd = { ConnectionNumber(wX11->display), POLLIN };

    while (!XPending(wX11->display))
    {
        if (!_wManagerPollPOSIX(&fd, 1, timeout))
            return false;
    }

    return true;
}

// Sends an EWMH or ICCCM event to the window manager
//
static void sendEventToWM(wManagerWindow* window, Atom type,
                          long a, long b, long c, long d, long e)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    XEvent event = { ClientMessage };
    event.xclient.window = x11->handle;
    event.xclient.format = 32; // Data is 32-bit longs
    event.xclient.message_type = type;
    event.xclient.data.l[0] = a;
    event.xclient.data.l[1] = b;
    event.xclient.data.l[2] = c;
    event.xclient.data.l[3] = d;
    event.xclient.data.l[4] = e;

    XSendEvent(wX11->display, wX11->root,
               False,
               SubstructureNotifyMask | SubstructureRedirectMask,
               &event);
}

void _wManagerGetWindowFrameSizeX11(wManagerWindow* window,
                                int* left, int* top,
                                int* right, int* bottom)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    long* extents = NULL;

    /*if (window->monitor || !window->decorated)
        return;*/

    if (wX11->NET_FRAME_EXTENTS == None)
        return;

    if (!_wManagerWindowVisibleX11(window) &&
        wX11->NET_REQUEST_FRAME_EXTENTS)
    {
        XEvent event;
        double timeout = 0.5;

        // Ensure _NET_FRAME_EXTENTS is set, allowing glfwGetWindowFrameSize to
        // function before the window is mapped
        sendEventToWM(window, wX11->NET_REQUEST_FRAME_EXTENTS,
                      0, 0, 0, 0, 0);

        // HACK: Use a timeout because earlier versions of some window managers
        //       (at least Unity, Fluxbox and Xfwm) failed to send the reply
        //       They have been fixed but broken versions are still in the wild
        //       If you are affected by this and your window manager is NOT
        //       listed above, PLEASE report it to their and our issue trackers
        while (!XCheckIfEvent(wX11->display,
                              &event,
                              isFrameExtentsEvent,
                              (XPointer) window))
        {
            if (!waitForX11Event(&timeout))
            {
                printf(
                                "X11: The window manager has a broken _NET_REQUEST_FRAME_EXTENTS implementation; please report this issue");
                return;
            }
        }
    }

    if (_wManagerGetWindowPropertyX11(x11->handle,
                                  wX11->NET_FRAME_EXTENTS,
                                  XA_CARDINAL,
                                  (unsigned char**) &extents) == 4)
    {
        if (left)
            *left = extents[0];
        if (top)
            *top = extents[2];
        if (right)
            *right = extents[1];
        if (bottom)
            *bottom = extents[3];
    }

    if (extents)
        XFree(extents);
}

void _wManagerGetWindowContentScaleX11(wManagerWindow* window, float* xscale, float* yscale)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    if (xscale)
        *xscale = wX11->contentScaleX;
    if (yscale)
        *yscale = wX11->contentScaleY;
}

void _wManagerIconifyWindowX11(wManagerWindow* window)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    if (x11->overrideRedirect)
    {
        // Override-redirect windows cannot be iconified or restored, as those
        // tasks are performed by the window manager
        printf("X11: Iconification of full screen windows requires a WM that supports EWMH full screen");
        return;
    }

    XIconifyWindow(wX11->display, x11->handle, wX11->screen);
    XFlush(wX11->display);
}

// Waits until a VisibilityNotify event arrives for the specified window or the
// timeout period elapses (ICCCM section 4.2.2)
//
static int32_t waitForVisibilityNotify(wManagerWindow* window)
{
    XEvent dummy;
    double timeout = 0.1;

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    while (!XCheckTypedWindowEvent(wX11->display,
                                   x11->handle,
                                   VisibilityNotify,
                                   &dummy))
    {
        if (!waitForX11Event(&timeout))
            return false;
    }

    return true;
}

void _wManagerRestoreWindowX11(wManagerWindow* window)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    if (x11->overrideRedirect)
    {
        // Override-redirect windows cannot be iconified or restored, as those
        // tasks are performed by the window manager
        printf("X11: Iconification of full screen windows requires a WM that supports EWMH full screen");
        return;
    }

    if (_wManagerWindowIconifiedX11(window))
    {
        XMapWindow(wX11->display, x11->handle);
        waitForVisibilityNotify(window);
    }
    else if (_wManagerWindowVisibleX11(window))
    {
        if (wX11->NET_WM_STATE &&
            wX11->NET_WM_STATE_MAXIMIZED_VERT &&
            wX11->NET_WM_STATE_MAXIMIZED_HORZ)
        {
            sendEventToWM(window,
                          wX11->NET_WM_STATE,
                          _NET_WM_STATE_REMOVE,
                          wX11->NET_WM_STATE_MAXIMIZED_VERT,
                          wX11->NET_WM_STATE_MAXIMIZED_HORZ,
                          1, 0);
        }
    }

    XFlush(wX11->display);
}

void _wManagerMaximizeWindowX11(wManagerWindow* window)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    if (!wX11->NET_WM_STATE ||
        !wX11->NET_WM_STATE_MAXIMIZED_VERT ||
        !wX11->NET_WM_STATE_MAXIMIZED_HORZ)
    {
        return;
    }

    if (_wManagerWindowVisibleX11(window))
    {
        sendEventToWM(window,
                    wX11->NET_WM_STATE,
                    _NET_WM_STATE_ADD,
                    wX11->NET_WM_STATE_MAXIMIZED_VERT,
                    wX11->NET_WM_STATE_MAXIMIZED_HORZ,
                    1, 0);
    }
    else
    {
        Atom* states = NULL;
        unsigned long count =
            _wManagerGetWindowPropertyX11(x11->handle,
                                      wX11->NET_WM_STATE,
                                      XA_ATOM,
                                      (unsigned char**) &states);

        // NOTE: We don't check for failure as this property may not exist yet
        //       and that's fine (and we'll create it implicitly with append)

        Atom missing[2] =
        {
            wX11->NET_WM_STATE_MAXIMIZED_VERT,
            wX11->NET_WM_STATE_MAXIMIZED_HORZ
        };
        unsigned long missingCount = 2;

        for (unsigned long i = 0;  i < count;  i++)
        {
            for (unsigned long j = 0;  j < missingCount;  j++)
            {
                if (states[i] == missing[j])
                {
                    missing[j] = missing[missingCount - 1];
                    missingCount--;
                }
            }
        }

        if (states)
            XFree(states);

        if (!missingCount)
            return;

        XChangeProperty(wX11->display, x11->handle,
                        wX11->NET_WM_STATE, XA_ATOM, 32,
                        PropModeAppend,
                        (unsigned char*) missing,
                        missingCount);
    }

    XFlush(wX11->display);
}

void _wManagerShowWindowX11(wManagerWindow* window)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    if (_wManagerWindowVisibleX11(window))
        return;

    XMapWindow(wX11->display, x11->handle);
    waitForVisibilityNotify(window);
}

void _wManagerHideWindowX11(wManagerWindow* window)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    XUnmapWindow(wX11->display, x11->handle);
    XFlush(wX11->display);
}

void _wManagerRequestWindowAttentionX11(wManagerWindow* window)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    if (!wX11->NET_WM_STATE || !wX11->NET_WM_STATE_DEMANDS_ATTENTION)
        return;

    sendEventToWM(window,
                  wX11->NET_WM_STATE,
                  _NET_WM_STATE_ADD,
                  wX11->NET_WM_STATE_DEMANDS_ATTENTION,
                  0, 1, 0);
}

void _wManagerFocusWindowX11(wManagerWindow* window)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    if (wX11->NET_ACTIVE_WINDOW)
        sendEventToWM(window, wX11->NET_ACTIVE_WINDOW, 1, 0, 0, 0, 0);
    else if (_wManagerWindowVisibleX11(window))
    {
        XRaiseWindow(wX11->display, x11->handle);
        XSetInputFocus(wX11->display, x11->handle,
                       RevertToParent, CurrentTime);
    }

    XFlush(wX11->display);
}

void _wManagerSetWindowMonitorX11(wManagerWindow* window,
                              void* monitor,
                              int xpos, int ypos,
                              int width, int height,
                              int refreshRate)
{

    /*wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    if (window->monitor == monitor)
    {
        if (monitor)
        {
            if (monitor->window == window)
                acquireMonitor(window);
        }
        else
        {
            if (!window->resizable)
                updateNormalHints(window, width, height);

            XMoveResizeWindow(wX11->display, x11->handle,
                              xpos, ypos, width, height);
        }

        XFlush(wX11->display);
        return;
    }

    if (window->monitor)
    {
        _wManagerSetWindowDecoratedX11(window, window->decorated);
        _wManagerSetWindowFloatingX11(window, window->floating);
        releaseMonitor(window);
    }

    //_wManagerInputWindowMonitor(window, monitor);
    updateNormalHints(window, width, height);

    if (window->monitor)
    {
        if (!_wManagerWindowVisibleX11(window))
        {
            XMapRaised(wX11->display, x11->handle);
            waitForVisibilityNotify(window);
        }

        updateWindowMode(window);
        acquireMonitor(window);
    }
    else
    {
        updateWindowMode(window);
        XMoveResizeWindow(wX11->display, x11->handle,
                          xpos, ypos, width, height);
    }

    XFlush(wX11->display);*/
}

int32_t _wManagerWindowIconifiedX11(wManagerWindow* window)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    return getWindowState(window) == IconicState;
}

int32_t _wManagerWindowVisibleX11(wManagerWindow* window)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    XWindowAttributes wa;
    XGetWindowAttributes(wX11->display, x11->handle, &wa);
    return wa.map_state == IsViewable;
}

uint32_t _wManagerWindowMaximizedX11(wManagerWindow* window)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    Atom* states;
    uint32_t maximized = false;

    if (!wX11->NET_WM_STATE ||
        !wX11->NET_WM_STATE_MAXIMIZED_VERT ||
        !wX11->NET_WM_STATE_MAXIMIZED_HORZ)
    {
        return maximized;
    }

    const unsigned long count =
        _wManagerGetWindowPropertyX11(x11->handle,
                                  wX11->NET_WM_STATE,
                                  XA_ATOM,
                                  (unsigned char**) &states);

    for (unsigned long i = 0;  i < count;  i++)
    {
        if (states[i] == wX11->NET_WM_STATE_MAXIMIZED_VERT ||
            states[i] == wX11->NET_WM_STATE_MAXIMIZED_HORZ)
        {
            maximized = true;
            break;
        }
    }

    if (states)
        XFree(states);

    return maximized;
}

uint32_t _wManagerWindowHoveredX11(wManagerWindow* window)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    Window w = wX11->root;
    while (w)
    {
        Window root;
        int rootX, rootY, childX, childY;
        unsigned int mask;

        _wManagerGrabErrorHandlerX11();

        const Bool result = XQueryPointer(wX11->display, w,
                                          &root, &w, &rootX, &rootY,
                                          &childX, &childY, &mask);

        _wManagerReleaseErrorHandlerX11();

        if (wX11->errorCode == BadWindow)
            w = wX11->root;
        else if (!result)
            return false;
        else if (w == x11->handle)
            return true;
    }

    return false;
}

uint32_t _wManagerFramebufferTransparentX11(wManagerWindow* window)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    if (!x11->transparent)
        return false;

    return XGetSelectionOwner(wX11->display, wX11->NET_WM_CM_Sx) != None;
}

void _wManagerSetWindowResizableX11(wManagerWindow* window, uint32_t enabled)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    int width, height;
    _wManagerGetWindowSizeX11(window, &width, &height);
    updateNormalHints(window, width, height);
}

void _wManagerSetWindowFloatingX11(wManagerWindow* window, uint32_t enabled)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    if (!wX11->NET_WM_STATE || !wX11->NET_WM_STATE_ABOVE)
        return;

    if (_wManagerWindowVisibleX11(window))
    {
        const long action = enabled ? _NET_WM_STATE_ADD : _NET_WM_STATE_REMOVE;
        sendEventToWM(window,
                      wX11->NET_WM_STATE,
                      action,
                      wX11->NET_WM_STATE_ABOVE,
                      0, 1, 0);
    }
    else
    {
        Atom* states = NULL;
        const unsigned long count =
            _wManagerGetWindowPropertyX11(x11->handle,
                                      wX11->NET_WM_STATE,
                                      XA_ATOM,
                                      (unsigned char**) &states);

        // NOTE: We don't check for failure as this property may not exist yet
        //       and that's fine (and we'll create it implicitly with append)

        if (enabled)
        {
            unsigned long i;

            for (i = 0;  i < count;  i++)
            {
                if (states[i] == wX11->NET_WM_STATE_ABOVE)
                    break;
            }

            if (i == count)
            {
                XChangeProperty(wX11->display, x11->handle,
                                wX11->NET_WM_STATE, XA_ATOM, 32,
                                PropModeAppend,
                                (unsigned char*) &wX11->NET_WM_STATE_ABOVE,
                                1);
            }
        }
        else if (states)
        {
            for (unsigned long i = 0;  i < count;  i++)
            {
                if (states[i] == wX11->NET_WM_STATE_ABOVE)
                {
                    states[i] = states[count - 1];
                    XChangeProperty(wX11->display, x11->handle,
                                    wX11->NET_WM_STATE, XA_ATOM, 32,
                                    PropModeReplace, (unsigned char*) states, count - 1);
                    break;
                }
            }
        }

        if (states)
            XFree(states);
    }

    XFlush(wX11->display);
}

void _wManagerSetWindowMousePassthroughX11(wManagerWindow* window, uint32_t enabled)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    if (!wX11->xshape.available)
        return;

    if (enabled)
    {
        Region region = XCreateRegion();
        XShapeCombineRegion(wX11->display, x11->handle,
                            ShapeInput, 0, 0, region, ShapeSet);
        XDestroyRegion(region);
    }
    else
    {
        XShapeCombineMask(wX11->display, x11->handle,
                          ShapeInput, 0, 0, None, ShapeSet);
    }
}

float _wManagerGetWindowOpacityX11(wManagerWindow* window)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    float opacity = 1.f;

    if (XGetSelectionOwner(wX11->display, wX11->NET_WM_CM_Sx))
    {
        CARD32* value = NULL;

        if (_wManagerGetWindowPropertyX11(x11->handle,
                                      wX11->NET_WM_WINDOW_OPACITY,
                                      XA_CARDINAL,
                                      (unsigned char**) &value))
        {
            opacity = (float) (*value / (double) 0xffffffffu);
        }

        if (value)
            XFree(value);
    }

    return opacity;
}

void _wManagerSetWindowOpacityX11(wManagerWindow* window, float opacity)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    const CARD32 value = (CARD32) (0xffffffffu * (double) opacity);
    XChangeProperty(wX11->display, x11->handle,
                    wX11->NET_WM_WINDOW_OPACITY, XA_CARDINAL, 32,
                    PropModeReplace, (unsigned char*) &value, 1);
}

// Drains available data from the empty event pipe
//
void drainEmptyEvents(void)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;

    for (;;)
    {
        char dummy[64];
        const ssize_t result = read(wX11->emptyEventPipe[0], dummy, sizeof(dummy));
        if (result == -1 && errno != EINTR)
            break;
    }
}

// Writes a byte to the empty event pipe
//
static void writeEmptyEvent(void)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;

    for (;;)
    {
        const char byte = 0;
        const ssize_t result = write(wX11->emptyEventPipe[1], &byte, 1);
        if (result == 1 || (result == -1 && errno != EINTR))
            break;
    }
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
                const int scancode = ((wManagerX11 *)_wMWindow.WindowData)->scancodes[key];
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

extern void _wManagerInputWindowCloseRequest(wManagerWindow* window);
extern void _wManagerInputWindowMaximize(wManagerWindow* window, int32_t maximized);
extern void _wManagerInputWindowDamage(wManagerWindow* window);
extern void _wManagerInputWindowFocus(wManagerWindow* window, int32_t focused);
extern void _wManagerInputWindowPos(wManagerWindow* window, int x, int y);
extern void _wManagerInputWindowSize(wManagerWindow* window, int width, int height);
extern void _wManagerInputFramebufferSize(wManagerWindow* window, int width, int height);

extern char** _wManagerParseUriList(char* text, int* count);

// Translates an X event modifier state mask
//
static int translateState(int state)
{
    int mods = 0;

    if (state & ShiftMask)
        mods |= ENGINE_MOD_SHIFT;
    if (state & ControlMask)
        mods |= ENGINE_MOD_CONTROL;
    if (state & Mod1Mask)
        mods |= ENGINE_MOD_ALT;
    if (state & Mod4Mask)
        mods |= ENGINE_MOD_SUPER;
    if (state & LockMask)
        mods |= ENGINE_MOD_CAPS_LOCK;
    if (state & Mod2Mask)
        mods |= ENGINE_MOD_NUM_LOCK;

    return mods;
}

// Translates an X11 key code to a GLFW key token
//
static int translateKey(int scancode)
{
    // Use the pre-filled LUT (see createKeyTables() in x11_init.c)
    if (scancode < 0 || (scancode > 255))
        return ENGINE_KEY_UNKNOWN;

    return ((wManagerX11 *)_wMWindow.WindowData)->keycodes[scancode];
}

static uint32_t decodeUTF8(const char** s)
{
    uint32_t codepoint = 0, count = 0;
    static const uint32_t offsets[] =
    {
        0x00000000u, 0x00003080u, 0x000e2080u,
        0x03c82080u, 0xfa082080u, 0x82082080u
    };

    do
    {
        codepoint = (codepoint << 6) + (unsigned char) **s;
        (*s)++;
        count++;
    } while ((**s & 0xc0) == 0x80);

    return codepoint - offsets[count - 1];
}

static Atom writeTargetToProperty(const XSelectionRequestEvent* request)
{
    wManagerX11 *wX11 = _wMWindow.WindowData;

    char* selectionString = NULL;
    const Atom formats[] = { wX11->UTF8_STRING, XA_STRING };
    const int formatCount = sizeof(formats) / sizeof(formats[0]);

    if (request->selection == wX11->PRIMARY)
        selectionString = wX11->primarySelectionString;
    else
        selectionString = wX11->clipboardString;

    if (request->property == None)
    {
        // The requester is a legacy client (ICCCM section 2.2)
        // We don't support legacy clients, so fail here
        return None;
    }

    if (request->target == wX11->TARGETS)
    {
        // The list of supported targets was requested

        const Atom targets[] = { wX11->TARGETS,
                                 wX11->MULTIPLE,
                                 wX11->UTF8_STRING,
                                 XA_STRING };

        XChangeProperty(wX11->display,
                        request->requestor,
                        request->property,
                        XA_ATOM,
                        32,
                        PropModeReplace,
                        (unsigned char*) targets,
                        sizeof(targets) / sizeof(targets[0]));

        return request->property;
    }

    if (request->target == wX11->MULTIPLE)
    {
        // Multiple conversions were requested

        Atom* targets;
        const unsigned long count =
            _wManagerGetWindowPropertyX11(request->requestor,
                                      request->property,
                                      wX11->ATOM_PAIR,
                                      (unsigned char**) &targets);

        for (unsigned long i = 0;  i < count;  i += 2)
        {
            int j;

            for (j = 0;  j < formatCount;  j++)
            {
                if (targets[i] == formats[j])
                    break;
            }

            if (j < formatCount)
            {
                XChangeProperty(wX11->display,
                                request->requestor,
                                targets[i + 1],
                                targets[i],
                                8,
                                PropModeReplace,
                                (unsigned char *) selectionString,
                                strlen(selectionString));
            }
            else
                targets[i + 1] = None;
        }

        XChangeProperty(wX11->display,
                        request->requestor,
                        request->property,
                        wX11->ATOM_PAIR,
                        32,
                        PropModeReplace,
                        (unsigned char*) targets,
                        count);

        XFree(targets);

        return request->property;
    }

    if (request->target == wX11->SAVE_TARGETS)
    {
        // The request is a check whether we support SAVE_TARGETS
        // It should be handled as a no-op side effect target

        XChangeProperty(wX11->display,
                        request->requestor,
                        request->property,
                        wX11->NULL_,
                        32,
                        PropModeReplace,
                        NULL,
                        0);

        return request->property;
    }

    // Conversion to a data target was requested

    for (int i = 0;  i < formatCount;  i++)
    {
        if (request->target == formats[i])
        {
            // The requested target is one we support

            XChangeProperty(wX11->display,
                            request->requestor,
                            request->property,
                            request->target,
                            8,
                            PropModeReplace,
                            (unsigned char *) selectionString,
                            strlen(selectionString));

            return request->property;
        }
    }

    // The requested target is not supported

    return None;
}


static void handleSelectionRequest(XEvent* event)
{
    const XSelectionRequestEvent* request = &event->xselectionrequest;

    XEvent reply = { SelectionNotify };
    reply.xselection.property = writeTargetToProperty(request);
    reply.xselection.display = request->display;
    reply.xselection.requestor = request->requestor;
    reply.xselection.selection = request->selection;
    reply.xselection.target = request->target;
    reply.xselection.time = request->time;

    XSendEvent(((wManagerX11 *)_wMWindow.WindowData)->display, request->requestor, False, 0, &reply);
}

static void processEvent(XEvent *event)
{
    wManagerX11 *wX11 = _wMWindow.WindowData;

    int keycode = 0;
    Bool filtered = False;

    // HACK: Save scancode as some IMs clear the field in XFilterEvent
    if (event->type == KeyPress || event->type == KeyRelease)
        keycode = event->xkey.keycode;

    filtered = XFilterEvent(event, None);

    if (wX11->randr.available)
    {
        if (event->type == wX11->randr.eventBase + RRNotify)
        {
            XRRUpdateConfiguration(event);
            //_wManagerPollMonitorsX11();
            return;
        }
    }

    if (wX11->xkb.available)
    {
        if (event->type == wX11->xkb.eventBase + XkbEventCode)
        {
            if (((XkbEvent*) event)->any.xkb_type == XkbStateNotify &&
                (((XkbEvent*) event)->state.changed & XkbGroupStateMask))
            {
                wX11->xkb.group = ((XkbEvent*) event)->state.group;
            }

            return;
        }
    }

    if (event->type == GenericEvent)
    {
        if (wX11->xi.available)
        {
            wManagerWindow* window = wX11->disabledCursorWindow;

            if (window &&
                window->rawMouseMotion &&
                event->xcookie.extension == wX11->xi.majorOpcode &&
                XGetEventData(wX11->display, &event->xcookie) &&
                event->xcookie.evtype == XI_RawMotion)
            {
                XIRawEvent* re = event->xcookie.data;
                if (re->valuators.mask_len)
                {
                    const double* values = re->raw_values;
                    double xpos = window->virtualCursorPosX;
                    double ypos = window->virtualCursorPosY;

                    if (XIMaskIsSet(re->valuators.mask, 0))
                    {
                        xpos += *values;
                        values++;
                    }

                    if (XIMaskIsSet(re->valuators.mask, 1))
                        ypos += *values;

                    _wManagerInputCursorPos(window, xpos, ypos);
                }
            }

            XFreeEventData(wX11->display, &event->xcookie);
        }

        return;
    }

    if (event->type == SelectionRequest)
    {
        handleSelectionRequest(event);
        return;
    }

    wManagerWindow* window = NULL;
    if (XFindContext(wX11->display,
                     event->xany.window,
                     wX11->context,
                     (XPointer*) &window) != 0)
    {
        // This is an event for a window that has already been destroyed
        return;
    }

    switch (event->type)
    {
        case ReparentNotify:
        {
            ((wManagerX11 *)window->WindowData)->parent = event->xreparent.parent;
            return;
        }

        case KeyPress:
        {
            const int key = translateKey(keycode);
            const int mods = translateState(event->xkey.state);
            const int plain = !(mods & (ENGINE_MOD_CONTROL | ENGINE_MOD_ALT));

            if (((wManagerX11 *)window->WindowData)->ic)
            {
                // HACK: Do not report the key press events duplicated by XIM
                //       Duplicate key releases are filtered out implicitly by
                //       the GLFW key repeat logic in _wManagerInputKey
                //       A timestamp per key is used to handle simultaneous keys
                // NOTE: Always allow the first event for each key through
                //       (the server never sends a timestamp of zero)
                // NOTE: Timestamp difference is compared to handle wrap-around
                Time diff = event->xkey.time - ((wManagerX11 *)window->WindowData)->keyPressTimes[keycode];
                if (diff == event->xkey.time || (diff > 0 && diff < ((Time)1 << 31)))
                {
                    if (keycode)
                        _wManagerInputKey(window, key, keycode, ENGINE_PRESS, mods);

                    ((wManagerX11 *)window->WindowData)->keyPressTimes[keycode] = event->xkey.time;
                }

                if (!filtered)
                {
                    int count;
                    Status status;
                    char buffer[100];
                    char* chars = buffer;

                    count = Xutf8LookupString(((wManagerX11 *)window->WindowData)->ic,
                                              &event->xkey,
                                              buffer, sizeof(buffer) - 1,
                                              NULL, &status);

                    if (status == XBufferOverflow)
                    {
                        chars = calloc(count + 1, 1);
                        count = Xutf8LookupString(((wManagerX11 *)window->WindowData)->ic,
                                                  &event->xkey,
                                                  chars, count,
                                                  NULL, &status);
                    }

                    if (status == XLookupChars || status == XLookupBoth)
                    {
                        const char* c = chars;
                        chars[count] = '\0';
                        while (c - chars < count)
                            _wManagerInputChar(window, decodeUTF8(&c), mods, plain);
                    }

                    if (chars != buffer)
                        free(chars);
                }
            }
            else
            {
                KeySym keysym;
                XLookupString(&event->xkey, NULL, 0, &keysym, NULL);

                _wManagerInputKey(window, key, keycode, ENGINE_PRESS, mods);

                const uint32_t codepoint = _wManagerKeySym2Unicode(keysym);
                if (codepoint != ENGINE_INVALID_CODEPOINT)
                    _wManagerInputChar(window, codepoint, mods, plain);
            }

            return;
        }

        case KeyRelease:
        {
            const int key = translateKey(keycode);
            const int mods = translateState(event->xkey.state);

            if (!wX11->xkb.detectable)
            {
                // HACK: Key repeat events will arrive as KeyRelease/KeyPress
                //       pairs with similar or identical time stamps
                //       The key repeat logic in _wManagerInputKey expects only key
                //       presses to repeat, so detect and discard release events
                if (XEventsQueued(wX11->display, QueuedAfterReading))
                {
                    XEvent next;
                    XPeekEvent(wX11->display, &next);

                    if (next.type == KeyPress &&
                        next.xkey.window == event->xkey.window &&
                        next.xkey.keycode == keycode)
                    {
                        // HACK: The time of repeat events sometimes doesn't
                        //       match that of the press event, so add an
                        //       epsilon
                        //       Toshiyuki Takahashi can press a button
                        //       16 times per second so it's fairly safe to
                        //       assume that no human is pressing the key 50
                        //       times per second (value is ms)
                        if ((next.xkey.time - event->xkey.time) < 20)
                        {
                            // This is very likely a server-generated key repeat
                            // event, so ignore it
                            return;
                        }
                    }
                }
            }

            _wManagerInputKey(window, key, keycode, ENGINE_RELEASE, mods);
            return;
        }

        case ButtonPress:
        {
            const int mods = translateState(event->xbutton.state);

            if (event->xbutton.button == Button1)
                _wManagerInputMouseClick(window, ENGINE_MOUSE_BUTTON_LEFT, ENGINE_PRESS, mods);
            else if (event->xbutton.button == Button2)
                _wManagerInputMouseClick(window, ENGINE_MOUSE_BUTTON_MIDDLE, ENGINE_PRESS, mods);
            else if (event->xbutton.button == Button3)
                _wManagerInputMouseClick(window, ENGINE_MOUSE_BUTTON_RIGHT, ENGINE_PRESS, mods);

            // Modern X provides scroll events as mouse button presses
            else if (event->xbutton.button == Button4)
                _wManagerInputScroll(window, 0.0, 1.0);
            else if (event->xbutton.button == Button5)
                _wManagerInputScroll(window, 0.0, -1.0);
            else if (event->xbutton.button == Button6)
                _wManagerInputScroll(window, 1.0, 0.0);
            else if (event->xbutton.button == Button7)
                _wManagerInputScroll(window, -1.0, 0.0);

            else
            {
                // Additional buttons after 7 are treated as regular buttons
                // We subtract 4 to fill the gap left by scroll input above
                _wManagerInputMouseClick(window,
                                     event->xbutton.button - Button1 - 4,
                                     ENGINE_PRESS,
                                     mods);
            }

            return;
        }

        case ButtonRelease:
        {
            const int mods = translateState(event->xbutton.state);

            if (event->xbutton.button == Button1)
            {
                _wManagerInputMouseClick(window,
                                     ENGINE_MOUSE_BUTTON_LEFT,
                                     ENGINE_RELEASE,
                                     mods);
            }
            else if (event->xbutton.button == Button2)
            {
                _wManagerInputMouseClick(window,
                                     ENGINE_MOUSE_BUTTON_MIDDLE,
                                     ENGINE_RELEASE,
                                     mods);
            }
            else if (event->xbutton.button == Button3)
            {
                _wManagerInputMouseClick(window,
                                     ENGINE_MOUSE_BUTTON_RIGHT,
                                     ENGINE_RELEASE,
                                     mods);
            }
            else if (event->xbutton.button > Button7)
            {
                // Additional buttons after 7 are treated as regular buttons
                // We subtract 4 to fill the gap left by scroll input above
                _wManagerInputMouseClick(window,
                                     event->xbutton.button - Button1 - 4,
                                     ENGINE_RELEASE,
                                     mods);
            }

            return;
        }

        case EnterNotify:
        {
            // XEnterWindowEvent is XCrossingEvent
            const int x = event->xcrossing.x;
            const int y = event->xcrossing.y;

            // HACK: This is a workaround for WMs (KWM, Fluxbox) that otherwise
            //       ignore the defined cursor for hidden cursor mode
            if (window->cursorMode == ENGINE_CURSOR_HIDDEN)
                updateCursorImage(window);

            _wManagerInputCursorEnter(window, true);
            _wManagerInputCursorPos(window, x, y);

            ((wManagerX11 *)window->WindowData)->lastCursorPosX = x;
            ((wManagerX11 *)window->WindowData)->lastCursorPosY = y;
            return;
        }

        case LeaveNotify:
        {
            _wManagerInputCursorEnter(window, false);
            return;
        }

        case MotionNotify:
        {
            const int x = event->xmotion.x;
            const int y = event->xmotion.y;

            if (x != ((wManagerX11 *)window->WindowData)->warpCursorPosX ||
                y != ((wManagerX11 *)window->WindowData)->warpCursorPosY)
            {
                // The cursor was moved by something other than GLFW

                if (window->cursorMode == ENGINE_CURSOR_DISABLED)
                {
                    if (wX11->disabledCursorWindow != window)
                        return;
                    if (window->rawMouseMotion)
                        return;

                    const int dx = x - ((wManagerX11 *)window->WindowData)->lastCursorPosX;
                    const int dy = y - ((wManagerX11 *)window->WindowData)->lastCursorPosY;

                    _wManagerInputCursorPos(window,
                                        window->virtualCursorPosX + dx,
                                        window->virtualCursorPosY + dy);
                }
                else
                    _wManagerInputCursorPos(window, x, y);
            }

            ((wManagerX11 *)window->WindowData)->lastCursorPosX = x;
            ((wManagerX11 *)window->WindowData)->lastCursorPosY = y;
            return;
        }

        case ConfigureNotify:
        {
            if (event->xconfigure.width != ((wManagerX11 *)window->WindowData)->width ||
                event->xconfigure.height != ((wManagerX11 *)window->WindowData)->height)
            {
                _wManagerInputFramebufferSize(window,
                                          event->xconfigure.width,
                                          event->xconfigure.height);

                _wManagerInputWindowSize(window,
                                     event->xconfigure.width,
                                     event->xconfigure.height);

                ((wManagerX11 *)window->WindowData)->width = event->xconfigure.width;
                ((wManagerX11 *)window->WindowData)->height = event->xconfigure.height;
            }

            int xpos = event->xconfigure.x;
            int ypos = event->xconfigure.y;

            // NOTE: ConfigureNotify events from the server are in local
            //       coordinates, so if we are reparented we need to translate
            //       the position into root (screen) coordinates
            if (!event->xany.send_event && ((wManagerX11 *)window->WindowData)->parent != wX11->root)
            {
                _wManagerGrabErrorHandlerX11();

                Window dummy;
                XTranslateCoordinates(wX11->display,
                                      ((wManagerX11 *)window->WindowData)->parent,
                                      wX11->root,
                                      xpos, ypos,
                                      &xpos, &ypos,
                                      &dummy);

                _wManagerReleaseErrorHandlerX11();
                if (wX11->errorCode == BadWindow)
                    return;
            }

            if (xpos != ((wManagerX11 *)window->WindowData)->xpos || ypos != ((wManagerX11 *)window->WindowData)->ypos)
            {
                _wManagerInputWindowPos(window, xpos, ypos);
                ((wManagerX11 *)window->WindowData)->xpos = xpos;
                ((wManagerX11 *)window->WindowData)->ypos = ypos;
            }

            return;
        }

        case ClientMessage:
        {
            // Custom client message, probably from the window manager

            if (filtered)
                return;

            if (event->xclient.message_type == None)
                return;

            if (event->xclient.message_type == wX11->WM_PROTOCOLS)
            {
                const Atom protocol = event->xclient.data.l[0];
                if (protocol == None)
                    return;

                if (protocol == wX11->WM_DELETE_WINDOW)
                {
                    // The window manager was asked to close the window, for
                    // example by the user pressing a 'close' window decoration
                    // button
                    _wManagerInputWindowCloseRequest(window);
                }
                else if (protocol == wX11->NET_WM_PING)
                {
                    // The window manager is pinging the application to ensure
                    // it's still responding to events

                    XEvent reply = *event;
                    reply.xclient.window = wX11->root;

                    XSendEvent(wX11->display, wX11->root,
                               False,
                               SubstructureNotifyMask | SubstructureRedirectMask,
                               &reply);
                }
            }
            else if (event->xclient.message_type == wX11->XdndEnter)
            {
                // A drag operation has entered the window
                unsigned long count;
                Atom* formats = NULL;
                const int32_t list = event->xclient.data.l[1] & 1;

                wX11->xdnd.source  = event->xclient.data.l[0];
                wX11->xdnd.version = event->xclient.data.l[1] >> 24;
                wX11->xdnd.format  = None;

                if (wX11->xdnd.version > _ENGINE_XDND_VERSION)
                    return;

                if (list)
                {
                    count = _wManagerGetWindowPropertyX11(wX11->xdnd.source,
                                                      wX11->XdndTypeList,
                                                      XA_ATOM,
                                                      (unsigned char**) &formats);
                }
                else
                {
                    count = 3;
                    formats = (Atom*) event->xclient.data.l + 2;
                }

                for (unsigned int i = 0;  i < count;  i++)
                {
                    if (formats[i] == wX11->text_uri_list)
                    {
                        wX11->xdnd.format = wX11->text_uri_list;
                        break;
                    }
                }

                if (list && formats)
                    XFree(formats);
            }
            else if (event->xclient.message_type == wX11->XdndDrop)
            {
                // The drag operation has finished by dropping on the window
                Time time = CurrentTime;

                if (wX11->xdnd.version > _ENGINE_XDND_VERSION)
                    return;

                if (wX11->xdnd.format)
                {
                    if (wX11->xdnd.version >= 1)
                        time = event->xclient.data.l[2];

                    // Request the chosen format from the source window
                    XConvertSelection(wX11->display,
                                      wX11->XdndSelection,
                                      wX11->xdnd.format,
                                      wX11->XdndSelection,
                                      ((wManagerX11 *)window->WindowData)->handle,
                                      time);
                }
                else if (wX11->xdnd.version >= 2)
                {
                    XEvent reply = { ClientMessage };
                    reply.xclient.window = wX11->xdnd.source;
                    reply.xclient.message_type = wX11->XdndFinished;
                    reply.xclient.format = 32;
                    reply.xclient.data.l[0] = ((wManagerX11 *)window->WindowData)->handle;
                    reply.xclient.data.l[1] = 0; // The drag was rejected
                    reply.xclient.data.l[2] = None;

                    XSendEvent(wX11->display, wX11->xdnd.source,
                               False, NoEventMask, &reply);
                    XFlush(wX11->display);
                }
            }
            else if (event->xclient.message_type == wX11->XdndPosition)
            {
                // The drag operation has moved over the window
                const int xabs = (event->xclient.data.l[2] >> 16) & 0xffff;
                const int yabs = (event->xclient.data.l[2]) & 0xffff;
                Window dummy;
                int xpos, ypos;

                if (wX11->xdnd.version > _ENGINE_XDND_VERSION)
                    return;

                XTranslateCoordinates(wX11->display,
                                      wX11->root,
                                      ((wManagerX11 *)window->WindowData)->handle,
                                      xabs, yabs,
                                      &xpos, &ypos,
                                      &dummy);

                _wManagerInputCursorPos(window, xpos, ypos);

                XEvent reply = { ClientMessage };
                reply.xclient.window = wX11->xdnd.source;
                reply.xclient.message_type = wX11->XdndStatus;
                reply.xclient.format = 32;
                reply.xclient.data.l[0] = ((wManagerX11 *)window->WindowData)->handle;
                reply.xclient.data.l[2] = 0; // Specify an empty rectangle
                reply.xclient.data.l[3] = 0;

                if (wX11->xdnd.format)
                {
                    // Reply that we are ready to copy the dragged data
                    reply.xclient.data.l[1] = 1; // Accept with no rectangle
                    if (wX11->xdnd.version >= 2)
                        reply.xclient.data.l[4] = wX11->XdndActionCopy;
                }

                XSendEvent(wX11->display, wX11->xdnd.source,
                           False, NoEventMask, &reply);
                XFlush(wX11->display);
            }

            return;
        }

        case SelectionNotify:
        {
            if (event->xselection.property == wX11->XdndSelection)
            {
                // The converted data from the drag operation has arrived
                char* data;
                const unsigned long result =
                    _wManagerGetWindowPropertyX11(event->xselection.requestor,
                                              event->xselection.property,
                                              event->xselection.target,
                                              (unsigned char**) &data);

                if (result)
                {
                    int count;
                    char** paths = _wManagerParseUriList(data, &count);

                    _wManagerInputDrop(window, count, (const char**) paths);

                    for (int i = 0;  i < count;  i++)
                        free(paths[i]);
                    free(paths);
                }

                if (data)
                    XFree(data);

                if (wX11->xdnd.version >= 2)
                {
                    XEvent reply = { ClientMessage };
                    reply.xclient.window = wX11->xdnd.source;
                    reply.xclient.message_type = wX11->XdndFinished;
                    reply.xclient.format = 32;
                    reply.xclient.data.l[0] = ((wManagerX11 *)window->WindowData)->handle;
                    reply.xclient.data.l[1] = result;
                    reply.xclient.data.l[2] = wX11->XdndActionCopy;

                    XSendEvent(wX11->display, wX11->xdnd.source,
                               False, NoEventMask, &reply);
                    XFlush(wX11->display);
                }
            }

            return;
        }

        case FocusIn:
        {
            if (event->xfocus.mode == NotifyGrab ||
                event->xfocus.mode == NotifyUngrab)
            {
                // Ignore focus events from popup indicator windows, window menu
                // key chords and window dragging
                return;
            }

            if (window->cursorMode == ENGINE_CURSOR_DISABLED)
                disableCursor(window);
            else if (window->cursorMode == ENGINE_CURSOR_CAPTURED)
                captureCursor(window);

            if (((wManagerX11 *)window->WindowData)->ic)
                XSetICFocus(((wManagerX11 *)window->WindowData)->ic);

            _wManagerInputWindowFocus(window, true);
            return;
        }

        case FocusOut:
        {
            if (event->xfocus.mode == NotifyGrab ||
                event->xfocus.mode == NotifyUngrab)
            {
                // Ignore focus events from popup indicator windows, window menu
                // key chords and window dragging
                return;
            }

            if (window->cursorMode == ENGINE_CURSOR_DISABLED)
                enableCursor(window);
            else if (window->cursorMode == ENGINE_CURSOR_CAPTURED)
                releaseCursor();

            if (((wManagerX11 *)window->WindowData)->ic)
                XUnsetICFocus(((wManagerX11 *)window->WindowData)->ic);

            /*if (window->monitor && window->autoIconify)
                _wManagerIconifyWindowX11(window);*/

            _wManagerInputWindowFocus(window, false);
            return;
        }

        case Expose:
        {
            _wManagerInputWindowDamage(window);
            return;
        }

        case PropertyNotify:
        {
            if (event->xproperty.state != PropertyNewValue)
                return;

            if (event->xproperty.atom == wX11->WM_STATE)
            {
                const int state = getWindowState(window);
                if (state != IconicState && state != NormalState)
                    return;

                const int32_t iconified = (state == IconicState);
                if (((wManagerX11 *)window->WindowData)->iconified != iconified)
                {
                    /*if (window->monitor)
                    {
                        if (iconified)
                            releaseMonitor(window);
                        else
                            acquireMonitor(window);
                    }*/

                    ((wManagerX11 *)window->WindowData)->iconified = iconified;
                    _wManagerInputWindowIconify(window, iconified);
                }
            }
            else if (event->xproperty.atom == wX11->NET_WM_STATE)
            {
                const int32_t maximized = _wManagerWindowMaximizedX11(window);
                if (((wManagerX11 *)window->WindowData)->maximized != maximized)
                {
                    ((wManagerX11 *)window->WindowData)->maximized = maximized;
                    _wManagerInputWindowMaximize(window, maximized);
                }
            }

            return;
        }

        case DestroyNotify:
            return;
    }
}


void _wManagerPollEventsX11(void)
{
    drainEmptyEvents();

    wManagerX11 *wX11 = _wMWindow.WindowData;

#if defined(ENGINE_BUILD_LINUX_JOYSTICK)
    if (_glfw.joysticksInitialized)
        _glfwDetectJoystickConnectionLinux();
#endif
    XPending(wX11->display);

    while (QLength(wX11->display))
    {
        XEvent event;
        XNextEvent(wX11->display, &event);
        processEvent(&event);
    }


    wManagerWindow* window = wX11->disabledCursorWindow;

    if (window)
    {
        wManagerX11 *x11 = window->WindowData;

        int width, height;
        _wManagerGetWindowSizeX11(window, &width, &height);

        // NOTE: Re-center the cursor only if it has moved since the last call,
        //       to avoid breaking glfwWaitEvents with MotionNotify
        if (x11->lastCursorPosX != width / 2 ||
            x11->lastCursorPosY != height / 2)
        {
            _wManagerSetCursorPosX11(window, width / 2, height / 2);
        }
    }

    XFlush(wX11->display);
}

// Wait for event data to arrive on any event file descriptor
// This avoids blocking other threads via the per-display Xlib lock that also
// covers GLX functions
//
uint32_t waitForAnyEvent(double* timeout)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;

    nfds_t count = 2;
    struct pollfd fds[3] =
    {
        { ConnectionNumber(wX11->display), POLLIN },
        { wX11->emptyEventPipe[0], POLLIN }
    };

#if defined(ENGINE_BUILD_LINUX_JOYSTICK)
    if (_glfw.joysticksInitialized)
        fds[count++] = (struct pollfd) { _glfw.linjs.inotify, POLLIN };
#endif

    while (!XPending(wX11->display))
    {
        if (!_wManagerPollPOSIX(fds, count, timeout))
            return false;

        for (int i = 1; i < count; i++)
        {
            if (fds[i].revents & POLLIN)
                return true;
        }
    }

    return true;
}

void _wManagerWaitEventsX11(void)
{
    waitForAnyEvent(NULL);
    _wManagerPollEventsX11();
}

void _wManagerWaitEventsTimeoutX11(double timeout)
{
    waitForAnyEvent(&timeout);
    _wManagerPollEventsX11();
}

void _wManagerPostEmptyEventX11(void)
{
    writeEmptyEvent();
}

void _wManagerGetRequiredInstanceExtensionsX11(char** extensions)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;

    if (!_wMInfo.vk.KHR_surface)
        return;

    if (!_wMInfo.vk.KHR_xcb_surface || !wX11->x11xcb.handle)
    {
        if (!_wMInfo.vk.KHR_xlib_surface)
            return;
    }

    extensions[0] = "VK_KHR_surface";

    // NOTE: VK_KHR_xcb_surface is preferred due to some early ICDs exposing but
    //       not correctly implementing VK_KHR_xlib_surface
    if (_wMInfo.vk.KHR_xcb_surface && wX11->x11xcb.handle)
        extensions[1] = "VK_KHR_xcb_surface";
    else
        extensions[1] = "VK_KHR_xlib_surface";
}

uint32_t _wManagerGetPhysicalDevicePresentationSupportX11(VkInstance instance,
                                                      VkPhysicalDevice device,
                                                      uint32_t queuefamily)
{

    wManagerX11 *wX11 = _wMWindow.WindowData;

    VisualID visualID = XVisualIDFromVisual(DefaultVisual(wX11->display,
                                                          wX11->screen));

    if (_wMInfo.vk.KHR_xcb_surface && wX11->x11xcb.handle)
    {
        PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR
            vkGetPhysicalDeviceXcbPresentationSupportKHR =
            (PFN_vkGetPhysicalDeviceXcbPresentationSupportKHR)
            vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceXcbPresentationSupportKHR");
        if (!vkGetPhysicalDeviceXcbPresentationSupportKHR)
        {
            printf("X11: Vulkan instance missing VK_KHR_xcb_surface extension\n");
            return false;
        }

        xcb_connection_t* connection = wX11->x11xcb.GetXCBConnection(wX11->display);
        if (!connection)
        {
            printf("X11: Failed to retrieve XCB connection\n");
            return false;
        }

        return vkGetPhysicalDeviceXcbPresentationSupportKHR(device,
                                                            queuefamily,
                                                            connection,
                                                            visualID);
    }
    else
    {
        PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR
            vkGetPhysicalDeviceXlibPresentationSupportKHR =
            (PFN_vkGetPhysicalDeviceXlibPresentationSupportKHR)
            vkGetInstanceProcAddr(instance, "vkGetPhysicalDeviceXlibPresentationSupportKHR");
        if (!vkGetPhysicalDeviceXlibPresentationSupportKHR)
        {
            printf("X11: Vulkan instance missing VK_KHR_xlib_surface extension\n");
            return false;
        }

        return vkGetPhysicalDeviceXlibPresentationSupportKHR(device,
                                                             queuefamily,
                                                             wX11->display,
                                                             visualID);
    }
}

uint32_t createNativeWindow(wManagerWindow* window,
                                   const _wManagerwndconfig* wndconfig,
                                   Visual* visual, int depth)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    int width = wndconfig->width;
    int height = wndconfig->height;

    if (wndconfig->scaleToMonitor)
    {
        width *= wX11->contentScaleX;
        height *= wX11->contentScaleY;
    }

    int xpos = 0, ypos = 0;

    if (wndconfig->xpos != ENGINE_ANY_POSITION && wndconfig->ypos != ENGINE_ANY_POSITION)
    {
        xpos = wndconfig->xpos;
        ypos = wndconfig->ypos;
    }

    // Create a colormap based on the visual used by the current context
    x11->colormap = XCreateColormap(wX11->display,
                                           wX11->root,
                                           visual,
                                           AllocNone);

    x11->transparent = _wManagerIsVisualTransparentX11(visual);

    XSetWindowAttributes wa = { 0 };
    wa.colormap = x11->colormap;
    wa.event_mask = StructureNotifyMask | KeyPressMask | KeyReleaseMask |
                    PointerMotionMask | ButtonPressMask | ButtonReleaseMask |
                    ExposureMask | FocusChangeMask | VisibilityChangeMask |
                    EnterWindowMask | LeaveWindowMask | PropertyChangeMask;

    _wManagerGrabErrorHandlerX11();

    x11->parent = wX11->root;
    x11->handle = XCreateWindow(wX11->display,
                                       wX11->root,
                                       xpos, ypos,
                                       width, height,
                                       0,      // Border width
                                       depth,  // Color depth
                                       InputOutput,
                                       visual,
                                       CWBorderPixel | CWColormap | CWEventMask,
                                       &wa);

    _wManagerReleaseErrorHandlerX11();

    if (!x11->handle)
    {
        printf("X11: Failed to create window\n");
        return false;
    }

    XSaveContext(wX11->display,
                 x11->handle,
                 wX11->context,
                 (XPointer) window);

    /*if (!wndconfig->decorated)
        _wManagerSetWindowDecoratedX11(window, false);*/

    /*if (wX11->NET_WM_STATE && !window->monitor)
    {
        Atom states[3];
        int count = 0;

        if (wndconfig->floating)
        {
            if (wX11->NET_WM_STATE_ABOVE)
                states[count++] = wX11->NET_WM_STATE_ABOVE;
        }

        if (wndconfig->maximized)
        {
            if (wX11->NET_WM_STATE_MAXIMIZED_VERT &&
                wX11->NET_WM_STATE_MAXIMIZED_HORZ)
            {
                states[count++] = wX11->NET_WM_STATE_MAXIMIZED_VERT;
                states[count++] = wX11->NET_WM_STATE_MAXIMIZED_HORZ;
                x11->maximized = true;
            }
        }

        if (count)
        {
            XChangeProperty(wX11->display, x11->handle,
                            wX11->NET_WM_STATE, XA_ATOM, 32,
                            PropModeReplace, (unsigned char*) states, count);
        }
    }*/

    // Declare the WM protocols supported by GLFW
    {
        Atom protocols[] =
        {
            wX11->WM_DELETE_WINDOW,
            wX11->NET_WM_PING
        };

        XSetWMProtocols(wX11->display, x11->handle,
                        protocols, sizeof(protocols) / sizeof(Atom));
    }

    // Declare our PID
    {
        const long pid = getpid();

        XChangeProperty(wX11->display,  x11->handle,
                        wX11->NET_WM_PID, XA_CARDINAL, 32,
                        PropModeReplace,
                        (unsigned char*) &pid, 1);
    }

    if (wX11->NET_WM_WINDOW_TYPE && wX11->NET_WM_WINDOW_TYPE_NORMAL)
    {
        Atom type = wX11->NET_WM_WINDOW_TYPE_NORMAL;
        XChangeProperty(wX11->display,  x11->handle,
                        wX11->NET_WM_WINDOW_TYPE, XA_ATOM, 32,
                        PropModeReplace, (unsigned char*) &type, 1);
    }

    // Set ICCCM WM_HINTS property
    {
        XWMHints* hints = XAllocWMHints();
        if (!hints)
        {
            printf("X11: Failed to allocate WM hints\n");
            return false;
        }

        hints->flags = StateHint;
        hints->initial_state = NormalState;

        XSetWMHints(wX11->display, x11->handle, hints);
        XFree(hints);
    }

    // Set ICCCM WM_NORMAL_HINTS property
    {
        XSizeHints* hints = XAllocSizeHints();
        if (!hints)
        {
            printf("X11: Failed to allocate size hints\n");
            return false;
        }

        if (!wndconfig->resizable)
        {
            hints->flags |= (PMinSize | PMaxSize);
            hints->min_width  = hints->max_width  = width;
            hints->min_height = hints->max_height = height;
        }

        // HACK: Explicitly setting PPosition to any value causes some WMs, notably
        //       Compiz and Metacity, to honor the position of unmapped windows
        if (wndconfig->xpos != ENGINE_ANY_POSITION && wndconfig->ypos != ENGINE_ANY_POSITION)
        {
            hints->flags |= PPosition;
            hints->x = 0;
            hints->y = 0;
        }

        hints->flags |= PWinGravity;
        hints->win_gravity = StaticGravity;

        XSetWMNormalHints(wX11->display, x11->handle, hints);
        XFree(hints);
    }

    // Set ICCCM WM_CLASS property
    {
        XClassHint* hint = XAllocClassHint();

        if (strlen(wndconfig->x11.instanceName) &&
            strlen(wndconfig->x11.className))
        {
            hint->res_name = (char*) wndconfig->x11.instanceName;
            hint->res_class = (char*) wndconfig->x11.className;
        }
        else
        {
            const char* resourceName = getenv("RESOURCE_NAME");
            if (resourceName && strlen(resourceName))
                hint->res_name = (char*) resourceName;
            else if (strlen(wndconfig->title))
                hint->res_name = (char*) wndconfig->title;
            else
                hint->res_name = (char*) "glfw-application";

            if (strlen(wndconfig->title))
                hint->res_class = (char*) wndconfig->title;
            else
                hint->res_class = (char*) "GLFW-Application";
        }

        XSetClassHint(wX11->display, x11->handle, hint);
        XFree(hint);
    }

    // Announce support for Xdnd (drag and drop)
    {
        const Atom version = _ENGINE_XDND_VERSION;
        XChangeProperty(wX11->display, x11->handle,
                        wX11->XdndAware, XA_ATOM, 32,
                        PropModeReplace, (unsigned char*) &version, 1);
    }

    if (wX11->im)
        _wManagerCreateInputContextX11(window);

    _wManagerSetWindowTitleX11(window, wndconfig->title);
    _wManagerGetWindowPosX11(window, &x11->xpos, &x11->ypos);
    _wManagerGetWindowSizeX11(window, &x11->width, &x11->height);

    return true;
}

uint32_t _wManagerCreateWindowX11(wManagerWindow* window,
                              const _wManagerwndconfig* wndconfig,
                              const _wManagerctxconfig* ctxconfig,
                              const _wManagerfbconfig* fbconfig)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    Visual* visual = NULL;
    int depth;

    /*if (ctxconfig->client != ENGINE_NO_API)
    {
        if (ctxconfig->source == ENGINE_NATIVE_CONTEXT_API)
        {
            if (!_glfwInitGLX())
                return false;
            if (!_glfwChooseVisualGLX(wndconfig, ctxconfig, fbconfig, &visual, &depth))
                return false;
        }
        else if (ctxconfig->source == ENGINE_EGL_CONTEXT_API)
        {
            if (!_glfwInitEGL())
                return false;
            if (!_glfwChooseVisualEGL(wndconfig, ctxconfig, fbconfig, &visual, &depth))
                return false;
        }
        else if (ctxconfig->source == ENGINE_OSMESA_CONTEXT_API)
        {
            if (!_glfwInitOSMesa())
                return false;
        }
    }*/

    if (!visual)
    {
        visual = DefaultVisual(wX11->display, wX11->screen);
        depth = DefaultDepth(wX11->display, wX11->screen);
    }

    if (!createNativeWindow(window, wndconfig, visual, depth))
        return false;

    /*if (ctxconfig->client != ENGINE_NO_API)
    {
        if (ctxconfig->source == ENGINE_NATIVE_CONTEXT_API)
        {
            if (!_glfwCreateContextGLX(window, ctxconfig, fbconfig))
                return false;
        }
        else if (ctxconfig->source == ENGINE_EGL_CONTEXT_API)
        {
            if (!_glfwCreateContextEGL(window, ctxconfig, fbconfig))
                return false;
        }
        else if (ctxconfig->source == ENGINE_OSMESA_CONTEXT_API)
        {
            if (!_glfwCreateContextOSMesa(window, ctxconfig, fbconfig))
                return false;
        }

        if (!_glfwRefreshContextAttribs(window, ctxconfig))
            return false;
    }*/

    if (wndconfig->mousePassthrough)
        _wManagerSetWindowMousePassthroughX11(window, true);

    /*if (window->monitor)
    {
        _glfwShowWindowX11(window);
        updateWindowMode(window);
        acquireMonitor(window);

        if (wndconfig->centerCursor)
            _wManagerCenterCursorInContentArea(window);
    }
    else*/
    {
        if (wndconfig->visible)
        {
            _wManagerShowWindowX11(window);
            if (wndconfig->focused)
                _wManagerFocusWindowX11(window);
        }
    }

    XFlush(wX11->display);
    return true;
}

void _wManagerDestroyWindowX11(wManagerWindow* window)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    if (wX11->disabledCursorWindow == window)
        enableCursor(window);

    /*if (window->monitor)
        releaseMonitor(window);*/

    if (x11->ic)
    {
        XDestroyIC(x11->ic);
        x11->ic = NULL;
    }

    /*if (window->context.destroy)
        window->context.destroy(window);*/

    if (x11->handle)
    {
        XDeleteContext(wX11->display, x11->handle, wX11->context);
        XUnmapWindow(wX11->display, x11->handle);
        XDestroyWindow(wX11->display, x11->handle);
        x11->handle = (Window) 0;
    }

    if (x11->colormap)
    {
        XFreeColormap(wX11->display, x11->colormap);
        x11->colormap = (Colormap) 0;
    }

    XFlush(wX11->display);
}

const char* _wManagerGetVulkanResultString(VkResult result);

VkResult _wManagerCreateWindowSurfaceX11(VkInstance instance,
                                     wManagerWindow* window,
                                     const VkAllocationCallbacks* allocator,
                                     VkSurfaceKHR* surface)
{

    wManagerX11 *x11 = window->WindowData;
    wManagerX11 *wX11 = _wMWindow.WindowData;

    if (_wMInfo.vk.KHR_xcb_surface && wX11->x11xcb.handle)
    {
        VkResult err;
        VkXcbSurfaceCreateInfoKHR sci;
        PFN_vkCreateXcbSurfaceKHR vkCreateXcbSurfaceKHR;

        xcb_connection_t* connection = wX11->x11xcb.GetXCBConnection(wX11->display);
        if (!connection)
        {
            printf("X11: Failed to retrieve XCB connection\n");
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        vkCreateXcbSurfaceKHR = (PFN_vkCreateXcbSurfaceKHR)
            vkGetInstanceProcAddr(instance, "vkCreateXcbSurfaceKHR");
        if (!vkCreateXcbSurfaceKHR)
        {
            printf("X11: Vulkan instance missing VK_KHR_xcb_surface extension\n");
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        memset(&sci, 0, sizeof(sci));
        sci.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
        sci.connection = connection;
        sci.window = x11->handle;

        err = vkCreateXcbSurfaceKHR(instance, &sci, allocator, surface);
        if (err)
        {
            printf("X11: Failed to create Vulkan XCB surface: %s\n", _wManagerGetVulkanResultString(err));
        }

        return err;
    }
    else
    {
        VkResult err;
        VkXlibSurfaceCreateInfoKHR sci;
        PFN_vkCreateXlibSurfaceKHR vkCreateXlibSurfaceKHR;

        vkCreateXlibSurfaceKHR = (PFN_vkCreateXlibSurfaceKHR)
            vkGetInstanceProcAddr(instance, "vkCreateXlibSurfaceKHR");
        if (!vkCreateXlibSurfaceKHR)
        {
            printf("X11: Vulkan instance missing VK_KHR_xlib_surface extension\n");
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }

        memset(&sci, 0, sizeof(sci));
        sci.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
        sci.dpy = wX11->display;
        sci.window = x11->handle;

        err = vkCreateXlibSurfaceKHR(instance, &sci, allocator, surface);
        if (err)
        {
            printf("X11: Failed to create Vulkan X11 surface: %s\n",_wManagerGetVulkanResultString(err));
        }

        return err;
    }
}

void wManagerInitPlatform(wManagerWindow *window){

}

#endif
