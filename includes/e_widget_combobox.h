#ifndef E_WIDGET_COMBOBOX_H
#define E_WIDGET_COMBOBOX_H

#include "e_widget.h"
#include "e_widget_button.h"
#include "e_widget_list.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    EWidget widget;
    EWidgetButton button;
    EWidgetList list;
    bool show;
    int currId;
    int size_x;
    int size_y;
} EWidgetCombobox;

void ComboboxWidgetInit(EWidgetCombobox *combobox, EWidget *parent);
void ComboboxWidgetAddItem(EWidgetCombobox *combobox, const char* text);

#ifdef __cplusplus
}
#endif

#endif // E_WIDGET_COMBOBOX_H
