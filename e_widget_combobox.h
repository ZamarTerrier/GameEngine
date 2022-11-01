#ifndef E_WIDGET_COMBOBOX_H
#define E_WIDGET_COMBOBOX_H

#include "e_widget.h"
#include "e_widget_button.h"
#include "e_widget_list.h"

typedef struct{
    EWidget widget;
    EWidgetButton button;
    EWidgetList list;
    bool show;
    int currId;
} EWidgetCombobox;

void ComboboxWidgetInit(EWidgetCombobox *combobox, EWidget *parent);
void ComboboxWidgetAddItem(EWidgetCombobox *combobox, const char* text);

#endif // E_WIDGET_COMBOBOX_H
