#ifndef GAMENGINE_H
#define GAMENGINE_H

#include "stdinclude.h"

#include "e_resource.h"
#include "debuger.h"
#include "device.h"
#include "window.h"
#include "swapchain.h"
#include "pipeline.h"
#include "buffers.h"
#include "texture.h"

void initVulkan();

void initEngine(int width, int height, const char* name);

void EngineFixedCursorCenter();

void EngineGetCursorPos(int *xpos, int *ypos);
void EngineSetCursorPos(float xpos, float ypos);
void EngineHideCursor(char state);

int EngineGetMousePress(int Key);
int EngineGetKeyPress(int Key);

void EngineSetMouseKeyCallback(void *callback);
void EngineSetKeyCallback(void *callback);
void EngineSetCharCallback(void *callback);

void EngineSetCursorPoscallback(void * callback);

void EngineSetRecreateFunc(void *func);

void cleanupSwapChain();

void recreateSwapChain();

void createSyncObjects();

void engineLoop();

void drawFrame();

void engDraw(void* obj);

void updateUniformBuffer();

void cleanUp();

#endif
