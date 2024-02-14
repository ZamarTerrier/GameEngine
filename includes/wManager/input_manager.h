#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include "window_manager.h"

void _wManagerInputKey(wManagerWindow* window, int key, int scancode, int action, int mods);
void _wManagerInputChar(wManagerWindow* window, uint32_t codepoint, int mods, int32_t plain);

void _wManagerInputCursorEnter(wManagerWindow* window, int32_t entered);
void _wManagerInputCursorPos(wManagerWindow* window, double xpos, double ypos);

void _wManagerInputMouseClick(wManagerWindow* window, int button, int action, int mods);
void _wManagerInputScroll(wManagerWindow* window, double xoffset, double yoffset);

void _wManagerInputDrop(wManagerWindow* window, int count, const char** paths);

#endif // INPUT_MANAGER_H
