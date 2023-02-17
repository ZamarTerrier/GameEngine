#ifndef E_WIDGET_LIST_H
#define E_WIDGET_LIST_H

#include "engine_includes.h"

#include "e_widget.h"
#include "e_widget_button.h"
#include "e_widget_scroll.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    EWidget widget;
    int size_x;
    int size_y;
    int size;
} EWidgetList;

void ListWidgetInit(EWidgetList *list, int size_x, int size_y, EWidget *parent);

void ListWidgetSetColor(EWidgetList *list, vec4 color);
EWidgetButton *ListWidgetAddItem(EWidgetList *list, const char *text);
void ListWidgetRemoveItem(EWidgetList *list, int num);

#ifdef __cplusplus
}
#endif

#endif // E_WIDGET_LIST_H
