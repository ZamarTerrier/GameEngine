#include "e_widget_combobox.h"

int ComboboxWidgetPressMain(EWidget* widget, void* entry, void *arg){

    EWidgetCombobox *cmb = (EWidgetCombobox *)widget->parent;

    if(!cmb->show)
    {
        cmb->show = true;
        Transform2DSetScale(&cmb->widget, cmb->size_x, (cmb->list.size + 1) * cmb->size_y);
    }else{
        cmb->show = false;
        Transform2DSetScale(&cmb->widget, cmb->size_x, cmb->size_y);
    }

    WidgetConfirmTrigger(cmb, ENGINE_WIDGET_TRIGGER_COMBOBOX_PRESS, NULL);

    return 0;
}

int ComboboxWidgetPressSub(EWidget* widget, int id, void *arg){

    EWidgetList *list = widget;
    EWidgetCombobox *cmb = (EWidgetCombobox *) widget->parent;

    ChildStack *child = WidgetFindChild(list, id);

    if(child == NULL)
        return;

    EWidgetButton *butt = child->node;

    char *temp[1024];
    TextWidgetGetText(&butt->text, temp);

    TextObjectSetTextU8(&cmb->button.text, temp);
    cmb->currId = id;

    if(!cmb->show)
    {
        cmb->show = true;
        Transform2DSetScale(&cmb->widget, cmb->size_x, (cmb->list.size + 1) * cmb->size_y);
    }else{
        cmb->show = false;
        Transform2DSetScale(&cmb->widget, cmb->size_x, cmb->size_y);
    }

    WidgetConfirmTrigger(cmb, ENGINE_WIDGET_TRIGGER_COMBOBOX_CHANGE_SELLECTED_ITEM, id);

    return 0;
}

void ComboboxWidgetInit(EWidgetCombobox *combobox, EWidget *parent){

    WidgetInit(&combobox->widget, NULL, parent);

    memcpy(combobox->widget.go.name, "Combobox", 8);
    combobox->widget.type = ENGINE_WIDGET_TYPE_COMBOBOX;

    ButtonWidgetInit(&combobox->button, " ", &combobox->widget);
    ButtonWidgetSetColor(&combobox->button, 0.4, 0.4, 0.4);

    combobox->size_x = 100;
    combobox->size_y = 30;
    combobox->currId = -1;

    Transform2DSetScale(&combobox->widget, combobox->size_x, combobox->size_y);
    Transform2DSetScale(&combobox->button, combobox->size_x, combobox->size_y);

    ListWidgetInit(&combobox->list, combobox->size_x, combobox->size_y, &combobox->widget);
    WidgetConnect(&combobox->button, ENGINE_WIDGET_TRIGGER_BUTTON_PRESS, ComboboxWidgetPressMain,  NULL);
    WidgetConnect(&combobox->list, ENGINE_WIDGET_TRIGGER_LIST_PRESS_ITEM, ComboboxWidgetPressSub,  NULL);

    Transform2DSetPosition(&combobox->list, 0, combobox->size_y * 2);
    combobox->show = false;

}

void ComboboxWidgetAddItem(EWidgetCombobox *combobox, const char* text){
    EWidgetButton *butt = ListWidgetAddItem(&combobox->list, text);

    butt->widget.widget_flags |= ENGINE_FLAG_WIDGET_ALLOCATED;
    Transform2DSetScale(&combobox->list, combobox->size_x, (combobox->list.size + 1) * combobox->size_y);
}
