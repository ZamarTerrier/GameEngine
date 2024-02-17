#include "wManager/manager_includes.h"
#include "wManager/window_manager.h"
#include"wManager/input_manager.h"

#include "Data/e_resource_engine.h"

extern wManagerWindow _wMWindow;

void _wManagerInputWindowDamage(wManagerWindow* window)
{
    if (window->callbacks.refresh)
        window->callbacks.refresh((wManagerWindow*) window);
}

void _wManagerInputWindowPos(wManagerWindow* window, int x, int y)
{
    if (window->callbacks.pos)
        window->callbacks.pos((wManagerWindow*) window, x, y);
}

extern void _wManagerInputWindowFocus(wManagerWindow* window, int32_t focused);

void _wManagerInputWindowIconify(wManagerWindow *window, int iconified){

    if (window->callbacks.iconify)
        window->callbacks.iconify((wManagerWindow*) window, iconified);

}

void _wManagerInputWindowMaximize(wManagerWindow *window, int maximized){

    if (window->callbacks.maximize)
        window->callbacks.maximize((wManagerWindow*) window, maximized);

}

void _wManagerInputFramebufferSize(wManagerWindow *window,int width,int height){

    if (window->callbacks.fbsize)
        window->callbacks.fbsize((wManagerWindow*) window, width, height);

}
void _wManagerInputWindowSize(wManagerWindow *window, int width, int height){

    if (window->callbacks.fbsize)
        window->callbacks.fbsize((wManagerWindow*) window, width, height);

}

// Notifies shared code that the user wishes to close a window
//
void _wManagerInputWindowCloseRequest(wManagerWindow* window)
{
    _wMWindow.shouldClose = true;

    if (window->callbacks.close)
        window->callbacks.close((wManagerWindow*) window);
}

void _wManagerInputWindowMonitor(wManagerWindow* window, _wManagerMonitor* monitor)
{
    window->monitor = monitor;
}

