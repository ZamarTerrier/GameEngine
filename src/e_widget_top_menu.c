#include "e_widget_top_menu.h"

#include "e_widget_button.h"

void TopMenuWidgetFocus(EWidget *widget, void *entry, void *args)
{
    EWidget *menu = widget;

    int iter = 0;
    EWidget *child = WidgetFindChild(menu, iter)->node;
    EWidget *last = NULL;

    while (last != child) {
        iter ++;
        last = child;
        child = WidgetFindChild(menu, iter)->node;

        if(child->type == GUI_TYPE_LIST)
            child->widget_flags &= ~(ENGINE_FLAG_WIDGET_VISIBLE);
    }
}

void ToggleMenu(EWidget *widget, void *entry, EWidgetList *list)
{
    EWidget *menu = widget->parent->parent;

    int iter = 0;
    EWidget *child = WidgetFindChild(menu, iter)->node;
    EWidget *last = NULL;

    while (last != child) {
        iter ++;
        last = child;
        child = WidgetFindChild(menu, iter)->node;

        if(child->type == GUI_TYPE_LIST && child != list)
            child->widget_flags &= ~(ENGINE_FLAG_WIDGET_VISIBLE);
    }

    if(!(list->widget.widget_flags & ENGINE_FLAG_WIDGET_VISIBLE))
        list->widget.widget_flags |= ENGINE_FLAG_WIDGET_VISIBLE;
    else
        list->widget.widget_flags &= ~(ENGINE_FLAG_WIDGET_VISIBLE);
}

void TopMenuWidgetResize(EWidgetTopMenu *top_menu)
{
    if(top_menu->window != NULL)
    {
        vec2 size = Transform2DGetScale(&top_menu->window->widget);

        Transform2DSetScale(&top_menu->widget, size.x, size.y);
        Transform2DSetScale(&top_menu->top, size.x, 20);
        Transform2DSetPosition(&top_menu->top, 0, 0);
        Transform2DSetPosition(&top_menu->widget, 0, 20);
    }else{
        Transform2DSetScale(&top_menu->widget, WIDTH, HEIGHT);
        Transform2DSetScale(&top_menu->top, WIDTH, 20);
        Transform2DSetPosition(&top_menu->top, 0, 0);
        Transform2DSetPosition(&top_menu->widget, 0, 0);
    }
}

void TopMenuWidgetInit(EWidgetTopMenu *top_menu, EWidgetWindow *window)
{
    WidgetInit(&top_menu->widget, NULL, window);
    memcpy(top_menu->widget.go.name, "Widget_Menu", 10);
    WidgetInit(&top_menu->top, NULL, &top_menu->widget);
    top_menu->widget.transparent = 0.0f;
    top_menu->window = window;

    memset(top_menu->list, 0, 256 * sizeof(LinkedButtonList));
    top_menu->num_elems = 0;

    TopMenuWidgetResize(top_menu);

    WidgetConnect(&top_menu->widget, GUI_TRIGGER_WIDGET_FOCUS, TopMenuWidgetFocus, NULL);
}

int TopMenuWidgetAddMenu(EWidgetTopMenu *top_menu, char *name)
{
    top_menu->num_elems ++;

    EWidgetButton *b_menu = calloc(1, sizeof(EWidgetButton));
    ButtonWidgetInit(b_menu, name, &top_menu->top);

    Transform2DSetPosition(b_menu, (top_menu->num_elems - 1) * 240, 0);
    Transform2DSetScale(b_menu, 120, 20);

    top_menu->list[top_menu->num_elems - 1].button = b_menu;

    return top_menu->num_elems - 1;
}

EWidgetButton *TopMenuWidgetAddItem(EWidgetTopMenu *top_menu, int num_menu, char *name)
{
    if(top_menu->list[num_menu].list != NULL)
    {
        EWidgetButton *button = ListWidgetAddItem(top_menu->list[num_menu].list, name);
        WidgetConnect(button, GUI_TRIGGER_BUTTON_PRESS, ToggleMenu, top_menu->list[num_menu].list);

        return button;
    }

    EWidgetList *l_menu = calloc(1, sizeof(EWidgetList));

    EWidget *point;

    if(top_menu->window == NULL)
        point = &top_menu->widget;
    else
        point = top_menu->window;

    ListWidgetInit(l_menu, 110, 20, point);
    ListWidgetSetColor(l_menu, (vec4){ 0.6, 0.6, 0.6, 1.0});
    vec2 pos = Transform2DGetPosition(top_menu->list[top_menu->num_elems - 1].button);
    Transform2DSetPosition(l_menu, pos.x, 40);
    EWidgetButton *button = ListWidgetAddItem(l_menu, name);
    l_menu->widget.widget_flags &= ~(ENGINE_FLAG_WIDGET_VISIBLE);
    top_menu->list[num_menu].list = l_menu;
    WidgetConnect(top_menu->list[top_menu->num_elems - 1].button, GUI_TRIGGER_BUTTON_PRESS, ToggleMenu, top_menu->list[top_menu->num_elems - 1].list);
    WidgetConnect(button, GUI_TRIGGER_BUTTON_PRESS, ToggleMenu, top_menu->list[top_menu->num_elems - 1].list);
    return button;
}
