#include "e_widget_combobox.h"

int ComboboxWidgetPressMain(EWidget* widget, void* entry, EWidgetCombobox *cmb){

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

int ComboboxWidgetPressSub(EWidget* widget, int id, EWidgetCombobox *cmb){

    EWidgetList *list = widget;

    ChildStack *child = WidgetFindChild(list, id);

    if(child == NULL)
        return;

    EWidgetButton *butt = child->node;

    char temp[1024];

    TextWidgetGetText(&butt->text, temp);

    ButtonWidgetSetText(&cmb->button, temp);

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

void ComboboxWidgetInit(EWidgetCombobox *combobox, vec2 scale, DrawParam *dParam, EWidget *parent){

    WidgetInit(&combobox->widget, dParam, parent);
    WidgetAddDefault(&combobox->widget, dParam->render);
    GameObject2DInitDraw(&combobox->widget);

    memcpy(combobox->widget.go.name, "Combobox", 8);
    combobox->widget.type = ENGINE_WIDGET_TYPE_COMBOBOX;

    ButtonWidgetInit(&combobox->button, " ", dParam, &combobox->widget);
    ButtonWidgetSetColor(&combobox->button, 0.4, 0.4, 0.4);

    combobox->size_x = scale.x;
    combobox->size_y = scale.y;
    combobox->currId = -1;

    Transform2DSetScale(&combobox->widget, combobox->size_x, combobox->size_y);
    Transform2DSetScale(&combobox->button, combobox->size_x, combobox->size_y);

    ListWidgetInit(&combobox->list, combobox->size_x, combobox->size_y, dParam, &combobox->widget);
    WidgetConnect(&combobox->button, ENGINE_WIDGET_TRIGGER_BUTTON_PRESS, ComboboxWidgetPressMain,  combobox);
    WidgetConnect(&combobox->list, ENGINE_WIDGET_TRIGGER_LIST_PRESS_ITEM, ComboboxWidgetPressSub,  combobox);

    Transform2DSetPosition(&combobox->list, 0, combobox->size_y * 2);
    combobox->show = false;

}

void ComboboxWidgetAddItem(EWidgetCombobox *combobox, const char* text, DrawParam *dParam){
    EWidgetButton *butt = ListWidgetAddItem(&combobox->list, text, dParam);

    butt->widget.widget_flags |= ENGINE_FLAG_WIDGET_ALLOCATED;
    Transform2DSetScale(&combobox->list, combobox->size_x, (combobox->list.size + 1) * combobox->size_y);
}
