#ifndef E_WIDGET_LIST_H
#define E_WIDGET_LIST_H

#include "engine_includes.h"

#include "e_widget.h"
#include "e_widget_button.h"
#include "e_widget_scroll.h"

typedef struct{
    EWidget widget;
    EWidgetScroll scroll;
    int size_x;
    int size_y;
    int size;
} EWidgetList;

void ListWidgetInit(EWidgetList *list, int size_x, int size_y, EWidget *parent);

void ListWidgetSetColor(EWidgetList *list, vec4 color);
EWidgetButton *ListWidgetAddItem(EWidgetList *list, const char *text);
void ListWidgetRemoveItem(EWidgetList *list, int num);

#endif // E_WIDGET_LIST_H
