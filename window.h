#ifndef WINENGINE_H
#define WINENGINE_H

#include "stdinclude.h"
#include "debuger.h"
#include "resource.h"

bool checkValidationLayerSupport();

const char** getRequiredExtensions();

void initWindow();

static void framebufferResizeCallback(GLFWwindow* window, int width, int height);

void createInstance();

void createSurface();

vec2 getWindowSize();

#endif
