#ifndef E_WIDGET_SCROLL_H
#define E_WIDGET_SCROLL_H

#include "e_widget.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    EWidget widget;
    EWidget scroll;
    uint32_t width;
    uint32_t height;
    uint32_t scroll_size;
} EWidgetScroll;

void ScrollWidgetInit(EWidgetScroll *scroll, uint32_t width, uint32_t height, DrawParam *dParam, EWidget *parent);
void ScrollWidgetSetScrollSize(EWidgetScroll *scroll, float percent);
void ScrollWidgetUpdate(EWidgetScroll *scroll, void *list);

#ifdef __cplusplus
}
#endif

#endif // E_WIDGET_SCROLL_H
