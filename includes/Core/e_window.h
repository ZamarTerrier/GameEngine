#ifndef WINENGINE_H
#define WINENGINE_H

#include "Variabels/engine_includes.h"
#include "e_debuger.h"

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
