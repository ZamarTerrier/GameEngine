#ifndef E_WIDGET_WINDOW_H
#define E_WIDGET_WINDOW_H

#include "stdinclude.h"

#include "e_widget.h"
#include "e_widget_text.h"


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
    vec2 lastSize;
    vec2 lastPos;
}EWidgetWindow;

void WindowWidgetInit(EWidgetWindow *ww, char* name, vec2 size, DrawParam dParam, vec2 position);
void WindowWidgetShow(EWidgetWindow *ww);
void WindowWidgetHide(EWidgetWindow *ww);
void WindowWidgetUpdate(EWidgetWindow *ww);
void WindowWidgetDraw(EWidgetWindow *ww);

#endif // E_WIDGET_WINDOW_H
