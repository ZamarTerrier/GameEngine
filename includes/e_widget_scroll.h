#ifndef E_WIDGET_SCROLL_H
#define E_WIDGET_SCROLL_H

#include "e_widget.h"

typedef struct{
    EWidget widget;
    EWidget scroll;
} EWidgetScroll;

void ScrollWidgetInit(EWidgetScroll *scroll, DrawParam *dParam, EWidget *parent);
void ScrollWidgetSetScrollSize(EWidgetScroll *scroll, float percent);

#endif // E_WIDGET_SCROLL_H
