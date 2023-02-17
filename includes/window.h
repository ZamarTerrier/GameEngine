#ifndef WINENGINE_H
#define WINENGINE_H

#include "engine_includes.h"
#include "debuger.h"
#include "e_resource.h"

#ifdef __cplusplus
extern "C"
{
#endif

bool checkValidationLayerSupport();

const char** getRequiredExtensions();

void initWindow();

static void framebufferResizeCallback(void* window, int width, int height);

void createInstance();

void createSurface();

vec2 getWindowSize();

#ifdef __cplusplus
}
#endif

#endif
