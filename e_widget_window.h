#ifndef E_WIDGET_WINDOW_H
#define E_WIDGET_WINDOW_H

#include "stdinclude.h"

#include "e_widget.h"

typedef struct{
    EWidget widget;
    EWidget top;
    EWidget hide;
    EWidget resize;
    EWidget close;
}EWidgetWindow;

void WindowWidgetInit(EWidgetWindow *ww, DrawParam dParam, vec2 size, vec2 position);
void WindowWidgetUpdate(EWidgetWindow *ww);
void WindowWidgetDraw(EWidgetWindow *ww);

#endif // E_WIDGET_WINDOW_H
