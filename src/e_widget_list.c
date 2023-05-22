#include "e_widget_list.h"

#include "e_math.h"

int max_size = 2;

int ListWidgetPressItem(EWidget *widget, void *entry, int id){

    EWidgetList *list = widget->parent;

    EWidgetButton *button;

    for(int i=0;i < list->size;i++)
    {
        button = WidgetFindChild(list, i)->node;
        button->widget.color = button->selfColor;
    }

    button = widget;

    button->widget.color.x += 0.6;

    WidgetConfirmTrigger(list, ENGINE_WIDGET_TRIGGER_LIST_PRESS_ITEM, id);

    return -1;
}

void ListWidgetInit(EWidgetList *list, int size_x, int size_y, DrawParam *dParam, EWidget *parent){

    WidgetInit(list, NULL, parent);
    WidgetAddDefault(list, dParam->render);
    GameObject2DInitDraw(list);

    memcpy(list->widget.go.name, "Widget_List", 11);
    list->widget.type = ENGINE_WIDGET_TYPE_LIST;

    list->widget.color = vec4_f(0.4, 0.4, 0.4, 1.0);
    list->widget.transparent = 0.0f;

    list->size_x = size_x;
    list->size_y = size_y;

    Transform2DSetScale(&list->widget, list->size_x, list->size_y);
}

void ListWidgetSetColor(EWidgetList *list, vec4 color){

    list->widget.color = color;

    for(int i=0;i < list->size;i++)
    {
         ChildStack *child = WidgetFindChild(&list->widget, i);
         EWidgetButton *widget = (EWidgetButton *)child->node;

        widget->selfColor = widget->widget.color = color;
    }
}

//Max 256 for some times...
EWidgetButton *ListWidgetAddItem(EWidgetList *list, const char *text, DrawParam *dParam){
    list->size ++;

    EWidgetButton *item = (EWidgetButton *) calloc(1, sizeof(EWidgetButton));

    Transform2DSetScale(&list->widget, list->size_x, list->size_y * list->size);
    ButtonWidgetInit(item, text, dParam, &list->widget);
    item->widget.widget_flags |= ENGINE_FLAG_WIDGET_ALLOCATED;
    ButtonWidgetSetColor(item, list->widget.color.x, list->widget.color.y, list->widget.color.z);
    TextWidgetSetText(&item->text, text);

    item->widget.transparent = 0.5f;

    WidgetConnect(item, ENGINE_WIDGET_TRIGGER_BUTTON_PRESS, ListWidgetPressItem, list->size -1);

    for(int i=0;i < list->size;i++)
    {
         ChildStack *child = WidgetFindChild(&list->widget, i);

         Transform2DSetPosition(child->node, 0, i * (list->size_y * 2));
         Transform2DSetScale(child->node, list->size_x, list->size_y);
    }

    return item;
}

void ListWidgetRemoveItem(EWidgetList *list, int num){

    if(num + 1 > list->size)
        return;

    ChildStack *child = WidgetFindChild(&list->widget, num);

    if(child == NULL)
        return;

    if(child->next != NULL && child->before != NULL)
    {
        ChildStack *next = child->next;
        ChildStack *before = child->before;

        WidgetDestroy(child->node);
        child->node = NULL;

        free(child);
        next->before = before;
        before->next = next;

    }else if(child->next != NULL){
        WidgetDestroy(child->node);
        child->node = NULL;

        child->next->before = NULL;
        list->widget.child = child->next;
        free(child);

    }else if(child->before != NULL){
        WidgetDestroy(child->node);
        child->node = NULL;

        child->before->next = NULL;
        list->widget.last = child->before;

        free(child);

    }else{
        if(list->widget.child->node != NULL)
        {
            WidgetDestroy(list->widget.child->node);
            list->widget.child->node = NULL;
        }

        if(list->widget.child != NULL)
        {
            free(list->widget.child);
            list->widget.last = NULL;
            list->widget.child = NULL;
        }
    }

    list->size--;

    Transform2DSetScale(&list->widget, list->size_x, list->size_y * list->size);

    for(int i=0;i < list->size;i++)
    {
        child = WidgetFindChild(&list->widget, i);

        EWidget* widget = child->node;
        widget->callbacks.stack[2].args = i;
        Transform2DSetPosition(child->node, 0, i * (list->size_y * 2));
        Transform2DSetScale(child->node, list->size_x, list->size_y);
    }
}
