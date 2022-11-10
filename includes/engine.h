#ifndef GAMENGINE_H
#define GAMENGINE_H

#include "engine_includes.h"

void initVulkan();

void initEngine(int width, int height, const char* name);

void EngineFixedCursorCenter();

void EngineGetCursorPos(double *xpos, double *ypos);
void EngineSetCursorPos(float xpos, float ypos);
void EngineHideCursor(char state);

int EngineGetMousePress(int Key);
int EngineGetKeyPress(int Key);

void EngineSetMouseKeyCallback(void *callback);
void EngineSetKeyCallback(void *callback);
void EngineSetCharCallback(void *callback);

void EngineSetCursorPoscallback(void * callback);

void EngineSetRecreateFunc(void *func);

int EngineWindowIsClosed();

double EngineGetTime();

void EnginePoolEvents();

void EngineDeviceWaitIdle();

void cleanupSwapChain();

void recreateSwapChain();

void createSyncObjects();

void engineLoop();

void drawFrame();

void engDraw(void* obj);

void updateUniformBuffer();

void cleanUp();

#endif
