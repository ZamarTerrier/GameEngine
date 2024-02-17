#include "wManager/input_manager.h"
#include "wManager/manager_includes.h"

#include <stdbool.h>
#include <assert.h>

extern wManagerWindow _wMWindow;

// Internal key state used for sticky keys
#define _ENGINE_STICK 3

// Internal constants for gamepad mapping source types
#define _ENGINE_JOYSTICK_AXIS     1
#define _ENGINE_JOYSTICK_BUTTON   2
#define _ENGINE_JOYSTICK_HATBIT   3

#define ENGINE_MOD_MASK (ENGINE_MOD_SHIFT | \
                       ENGINE_MOD_CONTROL | \
                       ENGINE_MOD_ALT | \
                       ENGINE_MOD_SUPER | \
                       ENGINE_MOD_CAPS_LOCK |

void _wManagerCenterCursorInContentArea(wManagerWindow* window)
{
    int width, height;

    _wMWindow.platform.getWindowSize(window, &width, &height);
    _wMWindow.platform.setCursorPos(window, width / 2.0, height / 2.0);
}

// Notifies shared code of a physical key event
//
void _wManagerInputKey(wManagerWindow* window, int key, int scancode, int action, int mods)
{
    if (key >= 0 && key <= ENGINE_KEY_LAST)
    {
        int32_t repeated = false;

        if (action == ENGINE_RELEASE && window->keys[key] == ENGINE_RELEASE)
            return;

        if (action == ENGINE_PRESS && window->keys[key] == ENGINE_PRESS)
            repeated = true;

        if (action == ENGINE_RELEASE && window->stickyKeys)
            window->keys[key] = _ENGINE_STICK;
        else
            window->keys[key] = (char) action;

        if (repeated)
            action = ENGINE_REPEAT;
    }

    if (!window->lockKeyMods)
        mods &= ~(ENGINE_MOD_CAPS_LOCK | ENGINE_MOD_NUM_LOCK);

    if (window->callbacks.key)
        window->callbacks.key((wManagerWindow*) window, key, scancode, action, mods);
}

// Notifies shared code of a Unicode codepoint input event
// The 'plain' parameter determines whether to emit a regular character event
//
void _wManagerInputChar(wManagerWindow* window, uint32_t codepoint, int mods, int32_t plain)
{
    if (codepoint < 32 || (codepoint > 126 && codepoint < 160))
        return;

    if (!window->lockKeyMods)
        mods &= ~(ENGINE_MOD_CAPS_LOCK | ENGINE_MOD_NUM_LOCK);

    if (window->callbacks.charmods)
        window->callbacks.charmods((wManagerWindow*) window, codepoint, mods);

    if (plain)
    {
        if (window->callbacks.character)
            window->callbacks.character((wManagerWindow*) window, codepoint);
    }
}

void _wManagerInputCursorEnter(wManagerWindow* window, int32_t entered)
{
    if (window->callbacks.cursorEnter)
        window->callbacks.cursorEnter((wManagerWindow*) window, entered);
}

// Notifies shared code of a cursor motion event
// The position is specified in content area relative screen coordinates
//
void _wManagerInputCursorPos(wManagerWindow* window, double xpos, double ypos)
{

    if (window->virtualCursorPosX == xpos && window->virtualCursorPosY == ypos)
        return;

    window->virtualCursorPosX = xpos;
    window->virtualCursorPosY = ypos;

    if (window->callbacks.cursorPos)
        window->callbacks.cursorPos((wManagerWindow*) window, xpos, ypos);
}

void _wManagerInputMouseClick(wManagerWindow* window, int button, int action, int mods)
{
    if (button < 0 || (button > ENGINE_MOUSE_BUTTON_LAST))
        return;

    if (!window->lockKeyMods)
        mods &= ~(ENGINE_MOD_CAPS_LOCK | ENGINE_MOD_NUM_LOCK);

    if (action == ENGINE_RELEASE && window->stickyMouseButtons)
        window->mouseButtons[button] = _ENGINE_STICK;
    else
        window->mouseButtons[button] = (char) action;

    if (window->callbacks.mouseButton)
        window->callbacks.mouseButton((wManagerWindow*) window, button, action, mods);
}

// Notifies shared code of a scroll event
//
void _wManagerInputScroll(wManagerWindow* window, double xoffset, double yoffset)
{
    if (window->callbacks.scroll)
        window->callbacks.scroll((wManagerWindow*) window, xoffset, yoffset);
}

// Notifies shared code of files or directories dropped on a window
//
void _wManagerInputDrop(wManagerWindow* window, int count, const char** paths)
{
    if (window->callbacks.drop)
        window->callbacks.drop((wManagerWindow*) window, count, paths);
}
