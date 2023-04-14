#ifndef E_WIDGET_WINDOW_H
#define E_WIDGET_WINDOW_H

#include "engine_includes.h"

#include "e_widget.h"
#include "e_widget_text.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    EWidget widget;
    EWidget top;
    EWidget hide;
    EWidget resize;
    EWidget close;
    EWidgetText name;
    bool show;
    bool wasResize;
    bool wasHide;
    bool resizeble;
    vec2 lastSize;
    vec2 lastPos;
}EWidgetWindow;

void WindowWidgetInit(EWidgetWindow *ww, char* name, vec2 size, DrawParam *dParam, vec2 position);
void WindowWidgetShow(EWidgetWindow *ww);
void WindowWidgetHide(EWidgetWindow *ww);
void WindowWidgetUpdate(EWidgetWindow *ww);
void WindowWidgetDraw(EWidgetWindow *ww);
void WindowWidgetDestroy(EWidgetWindow *ww);

#ifdef __cplusplus
}
#endif

#endif // E_WIDGET_WINDOW_H
