#ifndef E_WIDGET_LIST_H
#define E_WIDGET_LIST_H

#include "stdinclude.h"

#include "e_widget.h"
#include "e_widget_button.h"

typedef struct{
    EWidget widget;
    uint32_t size;
} EWidgetList;

void ListWidgetInit(EWidgetList *list, EWidget *parent);

EWidgetButton *ListWidgetAddItem(EWidgetList *list, const char *text);
void ListWidgetRemoveItem(EWidgetList *list, int num);

#endif // E_WIDGET_LIST_H
