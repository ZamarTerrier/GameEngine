#ifndef GAMENGINE_H
#define GAMENGINE_H

#include "stdinclude.h"

#include "resource.h"
#include "debuger.h"
#include "device.h"
#include "window.h"
#include "swapchain.h"
#include "pipeline.h"
#include "buffers.h"
#include "texture.h"

void initVulkan();

void initEngine(int width, int height, const char* name);

void EngineGetcursorPos(int *xpos, int *ypos);
void EngineFixedCursorCenter();
void EngineSetCursorPos(float xpos, float ypos);
void EngineHideCursor(char state);
int EngineGetKeyPress(int Key);
void EngineSetKeyCallback(void *callback);
void EngineSetcursorPoscallback(void * callback);

void cleanupSwapChain();

void recreateSwapChain();

void createSyncObjects();

void engineLoop();

void drawFrame();

void engDraw(void* go);
void engDraw3D(void* arg);
void engDrawText(void* arg);

void updateUniformBuffer();

void cleanUp();

#endif
