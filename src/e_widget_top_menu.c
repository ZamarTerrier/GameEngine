#include "e_widget_top_menu.h"

#include "e_widget_button.h"

#include "e_resource_data.h"

int TopMenuWidgetFocus(EWidget *widget, void *entry, void *args)
{
    EWidget *menu= widget;

    if(menu->type != ENGINE_WIDGET_TYPE_MENU)
    {
        menu = menu->parent;

        while(menu->type != ENGINE_WIDGET_TYPE_MENU)
        {
            menu = menu->parent;

            if(menu == NULL)
                return;
        }
    }

    int iter = 0;
    ChildStack *child = WidgetFindChild(menu, iter);
    EWidget *temp_w;

    while (child != NULL) {

        temp_w = child->node;

        if(temp_w->type == ENGINE_WIDGET_TYPE_LIST)
            temp_w->widget_flags &= ~(ENGINE_FLAG_WIDGET_VISIBLE);

        child = WidgetFindChild(menu, iter);

        iter ++;
    }

    return 0;
}

int ToggleMenu(EWidget *widget, void *entry, EWidgetList *list)
{
    EWidget *menu = widget->parent;

    while(menu->type != ENGINE_WIDGET_TYPE_MENU)
    {
        menu = menu->parent;

        if(menu == NULL)
            return;
    }

    int iter = 0;
    ChildStack *child = WidgetFindChild(menu, iter);
    EWidget *temp_w;

    while (child != NULL) {

        temp_w = child->node;

        if(temp_w->type == ENGINE_WIDGET_TYPE_LIST && temp_w != list)
            temp_w->widget_flags &= ~(ENGINE_FLAG_WIDGET_VISIBLE);

        child = WidgetFindChild(menu, iter);

        iter ++;
    }

    if(!(list->widget.widget_flags & ENGINE_FLAG_WIDGET_VISIBLE))
        list->widget.widget_flags |= ENGINE_FLAG_WIDGET_VISIBLE;
    else
        list->widget.widget_flags &= ~(ENGINE_FLAG_WIDGET_VISIBLE);

    return 0;
}

int MenuPressItem(EWidget *widget, int id, void *arg)
{
    EWidget *menu = widget->parent;

    while(menu->type != ENGINE_WIDGET_TYPE_MENU)
    {
        menu = menu->parent;

        if(menu == NULL)
            return;
    }

    int iter = 0;
    ChildStack *child = WidgetFindChild(menu, iter);
    EWidget *temp_w;

    while (child != NULL) {

        temp_w = child->node;

        if(temp_w->type == ENGINE_WIDGET_TYPE_LIST)
            temp_w->widget_flags &= ~(ENGINE_FLAG_WIDGET_VISIBLE);

        child = WidgetFindChild(menu, iter);

        iter ++;
    }

    MenuData data;
    data.num_menu = WidgetFindIdChild(widget) - 1;
    data.elem_id = id;

    WidgetConfirmTrigger(menu, ENGINE_WIDGET_TRIGGER_MENU_PRESS_ITEM, &data);

    return 0;
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

void TopMenuWidgetInit(EWidgetTopMenu *top_menu, DrawParam *dParam,EWidgetWindow *window)
{
    WidgetInit(&top_menu->widget, NULL, window);
    WidgetAddDefault(&top_menu->widget, dParam->render);
    GameObject2DInitDraw(&top_menu->widget);

    memcpy(top_menu->widget.go.name, "Widget_Menu", 10);
    top_menu->widget.type = ENGINE_WIDGET_TYPE_MENU;

    WidgetInit(&top_menu->top, NULL, &top_menu->widget);
    WidgetAddDefault(&top_menu->top, dParam->render);
    GameObject2DInitDraw(&top_menu->top);
    top_menu->widget.transparent = 0.0f;
    top_menu->window = window;

    memset(top_menu->list, 0, 256 * sizeof(LinkedButtonList));
    top_menu->num_elems = 0;

    TopMenuWidgetResize(top_menu);

    WidgetConnect(&top_menu->widget, ENGINE_WIDGET_TRIGGER_WIDGET_FOCUS, TopMenuWidgetFocus, NULL);

    top_menu->render = dParam->render;
}

int TopMenuWidgetAddMenu(EWidgetTopMenu *top_menu, char *name, DrawParam *dParam)
{
    top_menu->num_elems ++;

    EWidgetButton *b_menu = calloc(1, sizeof(EWidgetButton));
    ButtonWidgetInit(b_menu, name, dParam, &top_menu->top);

    Transform2DSetPosition(b_menu, (top_menu->num_elems - 1) * 240, 0);
    Transform2DSetScale(b_menu, 120, 20);

    top_menu->list[top_menu->num_elems - 1].button = b_menu;

    return top_menu->num_elems - 1;
}

void TopMenuWidgetAddItem(EWidgetTopMenu *top_menu, int num_menu, char *name, DrawParam *dParam)
{
    EWidgetButton *button;

    if(top_menu->list[num_menu].list != NULL)
    {
        button = ListWidgetAddItem(top_menu->list[num_menu].list, name, dParam);

        return top_menu->list[num_menu].list;
    }

    EWidgetList *l_menu = calloc(1, sizeof(EWidgetList));

    EWidget *point;

    if(top_menu->window == NULL)
        point = &top_menu->widget;
    else
        point = top_menu->window;

    ListWidgetInit(l_menu, 110, 20, dParam, point);
    ListWidgetSetColor(l_menu, (vec4){ 0.6, 0.6, 0.6, 1.0});

    vec2 pos = Transform2DGetPosition(top_menu->list[top_menu->num_elems - 1].button);
    Transform2DSetPosition(l_menu, pos.x, 40);
    l_menu->widget.widget_flags &= ~(ENGINE_FLAG_WIDGET_VISIBLE);
    top_menu->list[num_menu].list = l_menu;

    button = ListWidgetAddItem(top_menu->list[num_menu].list, name, dParam);

    WidgetConnect(top_menu->list[top_menu->num_elems - 1].list, ENGINE_WIDGET_TRIGGER_LIST_PRESS_ITEM, MenuPressItem, NULL);
    WidgetConnect(top_menu->list[top_menu->num_elems - 1].button, ENGINE_WIDGET_TRIGGER_BUTTON_PRESS, ToggleMenu, top_menu->list[top_menu->num_elems - 1].list);

    return top_menu->list[num_menu].list;
}
