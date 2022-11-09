#include "e_widget_list.h"

int max_size = 2;

void ListWidgetPressItem(EWidget *widget, void *entry, int id){

    EWidgetList *list = widget->parent;

    WidgetConfirmTrigger(list, GUI_TRIGGER_LIST_PRESS_ITEM, id);
}

void ListWidgetInit(EWidgetList *list, int size_x, int size_y, EWidget *parent){

    DrawParam dParam = {};

    WidgetInit(list, dParam, parent);
    list->widget.color = (vec4){ 0.4, 0.4, 0.4, 1.0};

    list->size_x = size_x;
    list->size_y = size_y;

}

void ListWidgetSetColor(EWidgetList *list, vec4 color){

    list->widget.color = color;

    for(int i=0;i < list->size;i++)
    {
         ChildStack *child =WidgetFindChild(&list->widget, i);
         EWidgetButton *widget = (EWidgetButton *)child->node;

        widget->selfColor =  widget->widget.color = color;
    }
}

//Max 256 for some times...
EWidgetButton *ListWidgetAddItem(EWidgetList *list, const char *text){
    list->size ++;

    EWidgetButton *item = (EWidgetButton *) calloc(1, sizeof(EWidgetButton));

    Transform2DSetScale(&list->widget, list->size_x, list->size_y * list->size);
    ButtonWidgetInit(item, text, list->widget.color, &list->widget);
    TextWidgetSetText(&item->text, text);

    WidgetConnect(item, GUI_TRIGGER_BUTTON_PRESS, ListWidgetPressItem, list->size -1);

    for(int i=0;i < list->size;i++)
    {
         ChildStack *child =WidgetFindChild(&list->widget, i);

         Transform2DSetPosition(child->node, 0, i * (list->size_y * 2));
         Transform2DSetScale(child->node, list->size_x, list->size_y);
    }

    return item;
}

void ListWidgetRemoveItem(EWidgetList *list, int num){

    if(num + 1 > list->size)
        return;

    ChildStack *child =WidgetFindChild(&list->widget, num - 1);

    if(child->next != NULL)
    {
        WidgetDestroy(child->node);
        child->node = child->next->node;
        child->next = child->next->next;
    }else{
        WidgetDestroy(child->node);
        free(child);
        child = NULL;
    }

     list->size--;

    Transform2DSetScale(&list->widget, list->size_x, list->size_y * list->size);

    for(int i=0;i < list->size;i++)
    {
        child =WidgetFindChild(&list->widget, i);

        EWidget* widget = child->node;
        widget->callbacks.stack[2].args = i;
        Transform2DSetPosition(child->node, 0, i * (list->size_y * 2));
        Transform2DSetScale(child->node, list->size_x, list->size_y);
    }
}
