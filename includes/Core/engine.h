#ifndef GAMENGINE_H
#define GAMENGINE_H

#include "Variabels/engine_includes.h"

#ifdef __cplusplus
extern "C"
{
#endif

void EngineInitSystem(int width, int height, const char* name);

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

const char *EngineGetClipBoard();

void EngineSetClipBoardString(const char *string);

void EnginePoolEvents();

void EngineDeviceWaitIdle();

void EngineCleanupSwapChain();

void EnginereRecreateSwapChain();

void EngineCreateSyncobjects();

void EngineAcceptShadow(void *shadow, uint32_t count, uint32_t shadow_type);
void EngineSetRender(void *obj, uint32_t count);
void EngineDraw(void *obj);
void EngineLoop();
void EngineUpdateUniformBuffer();

void EngineCleanUp();


#ifdef __cplusplus
}
#endif

#endif
