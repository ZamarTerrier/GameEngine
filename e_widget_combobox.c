#include "e_widget_combobox.h"

void ComboboxWidgetPressMain(EWidget* widget, void* entry, void *arg){

    EWidgetCombobox *cmb = (EWidgetCombobox *)widget->parent;

    if(!cmb->show)
    {
        vec2 size = {60, (cmb->list.size + 1) * 20};
        cmb->show = true;
        Transform2DSetScale(&cmb->widget, size);
    }else{
        vec2 size = {60, 20};
        cmb->show = false;
        Transform2DSetScale(&cmb->widget, size);
    }

    WidgetConfirmTrigger(cmb, GUI_TRIGGER_COMBOBOX_PRESS, NULL);
}

void ComboboxWidgetPressSub(EWidget* widget, void* entry, int id){

    EWidgetCombobox *cmb = (EWidgetCombobox *) widget->parent->parent;
    EWidgetButton *butt = (EWidgetButton *)widget;

    ButtonWidgetSetText(&cmb->button, butt->text.tData.text);
    cmb->currId = id;

    if(!cmb->show)
    {
        vec2 size = {60, (cmb->list.size + 1) * 20};
        cmb->show = true;
        Transform2DSetScale(&cmb->widget, size);
    }else{
        vec2 size = {60, 20};
        cmb->show = false;
        Transform2DSetScale(&cmb->widget, size);
    }

    WidgetConfirmTrigger(cmb, GUI_TRIGGER_COMBOBOX_CHANGE_SELLECTED_ITEM, id);
}

void ComboboxWidgetInit(EWidgetCombobox *combobox, EWidget *parent){

    DrawParam param = {};

    WidgetInit(&combobox->widget, param, parent);
    ButtonWidgetInit(&combobox->button, "", &combobox->widget);
    vec2 size = {60, 20};
    Transform2DSetScale(&combobox->widget, size);
    Transform2DSetScale(&combobox->button, size);

    ListWidgetInit(&combobox->list, &combobox->widget);
    WidgetConnect(&combobox->button, GUI_TRIGGER_BUTTON_PRESS, ComboboxWidgetPressMain,  NULL);

    vec2 position = {0, 40};
    Transform2DSetPosition(&combobox->list, position);
    combobox->show = false;

}

void ComboboxWidgetAddItem(EWidgetCombobox *combobox, const char* text){
    EWidgetButton *butt = ListWidgetAddItem(&combobox->list, text);

    WidgetConnect(butt, GUI_TRIGGER_BUTTON_PRESS, ComboboxWidgetPressSub,  combobox->list.size - 1);

    vec2 size = {60, (combobox->list.size + 1) * 20};
    Transform2DSetScale(&combobox->list, size);
}
