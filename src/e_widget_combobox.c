#include "e_widget_combobox.h"

void ComboboxWidgetPressMain(EWidget* widget, void* entry, void *arg){

    EWidgetCombobox *cmb = (EWidgetCombobox *)widget->parent;

    if(!cmb->show)
    {
        cmb->show = true;
        Transform2DSetScale(&cmb->widget, 60, (cmb->list.size + 1) * 20);
    }else{
        cmb->show = false;
        Transform2DSetScale(&cmb->widget, 60, 20);
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
        cmb->show = true;
        Transform2DSetScale(&cmb->widget, 60, (cmb->list.size + 1) * 20);
    }else{
        cmb->show = false;
        Transform2DSetScale(&cmb->widget, 60, 20);
    }

    WidgetConfirmTrigger(cmb, GUI_TRIGGER_COMBOBOX_CHANGE_SELLECTED_ITEM, id);
}

void ComboboxWidgetInit(EWidgetCombobox *combobox, EWidget *parent){

    DrawParam param = {};

    WidgetInit(&combobox->widget, param, parent);
    ButtonWidgetInit(&combobox->button, L" ", (vec4){ 0.4, 0.4, 0.4, 1.0}, &combobox->widget);

    Transform2DSetScale(&combobox->widget, 60, 20);
    Transform2DSetScale(&combobox->button, 60, 20);

    ListWidgetInit(&combobox->list, 60, 20, &combobox->widget);
    WidgetConnect(&combobox->button, GUI_TRIGGER_BUTTON_PRESS, ComboboxWidgetPressMain,  NULL);

    Transform2DSetPosition(&combobox->list, 0, 40);
    combobox->show = false;

}

void ComboboxWidgetAddItem(EWidgetCombobox *combobox, const char* text){
    EWidgetButton *butt = ListWidgetAddItem(&combobox->list, text);

    WidgetConnect(butt, GUI_TRIGGER_BUTTON_PRESS, ComboboxWidgetPressSub,  combobox->list.size - 1);

    Transform2DSetScale(&combobox->list, 60, (combobox->list.size + 1) * 20);
}
