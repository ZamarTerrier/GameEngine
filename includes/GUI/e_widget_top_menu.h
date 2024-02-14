#ifndef E_WIDGET_TOP_MENU_H
#define E_WIDGET_TOP_MENU_H

#include "Variabels/engine_includes.h"

#include "e_widget.h"
#include "e_widget_button.h"
#include "e_widget_scroll.h"
#include "e_widget_list.h"
#include "e_widget_window.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    EWidgetButton *button;
    EWidgetList *list;
    bool submenu;
} LinkedButtonList;

typedef struct{
    int num_menu;
    int elem_id;
} MenuData;

typedef struct{
    EWidget widget;
    EWidget top;
    EWidgetWindow *window;
    LinkedButtonList list[256];
    uint32_t num_elems;
    void *render;
} EWidgetTopMenu;

void TopMenuWidgetResize(EWidgetTopMenu *top_menu);
void TopMenuWidgetInit(EWidgetTopMenu *top_menu, DrawParam *dParam, EWidgetWindow *window);
int TopMenuWidgetAddMenu(EWidgetTopMenu *top_menu, char *name, DrawParam *dParam);
void TopMenuWidgetAddItem(EWidgetTopMenu *top_menu, int num_menu, char *name, DrawParam *dParam);

#ifdef __cplusplus
}
#endif

#endif // E_WIDGET_TOP_MENU_H
