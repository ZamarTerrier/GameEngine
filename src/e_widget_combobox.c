#include "e_widget_combobox.h"

void ComboboxWidgetPressMain(EWidget* widget, void* entry, void *arg){

    EWidgetCombobox *cmb = (EWidgetCombobox *)widget->parent;

    if(!cmb->show)
    {
        cmb->show = true;
        Transform2DSetScale(&cmb->widget, cmb->size_x, (cmb->list.size + 1) * cmb->size_y);
    }else{
        cmb->show = false;
        Transform2DSetScale(&cmb->widget, cmb->size_x, cmb->size_y);
    }

    WidgetConfirmTrigger(cmb, GUI_TRIGGER_COMBOBOX_PRESS, NULL);
}

void ComboboxWidgetPressSub(EWidget* widget, void* entry, int id){

    EWidgetCombobox *cmb = (EWidgetCombobox *) widget->parent->parent;
    EWidgetButton *butt = (EWidgetButton *)widget;

    TextImageSetText(butt->text.tData.text, &cmb->button.text.widget, &cmb->button.text.tData);
    cmb->currId = id;

    if(!cmb->show)
    {
        cmb->show = true;
        Transform2DSetScale(&cmb->widget, cmb->size_x, (cmb->list.size + 1) * cmb->size_y);
    }else{
        cmb->show = false;
        Transform2DSetScale(&cmb->widget, cmb->size_x, cmb->size_y);
    }

    WidgetConfirmTrigger(cmb, GUI_TRIGGER_COMBOBOX_CHANGE_SELLECTED_ITEM, id);
}

void ComboboxWidgetInit(EWidgetCombobox *combobox, EWidget *parent){

    WidgetInit(&combobox->widget, NULL, parent);
    ButtonWidgetInit(&combobox->button, " ", (vec4){ 0.4, 0.4, 0.4, 1.0}, &combobox->widget);

    combobox->size_x = 100;
    combobox->size_y = 30;

    Transform2DSetScale(&combobox->widget, combobox->size_x, combobox->size_y);
    Transform2DSetScale(&combobox->button, combobox->size_x, combobox->size_y);

    ListWidgetInit(&combobox->list, combobox->size_x, combobox->size_y, &combobox->widget);
    WidgetConnect(&combobox->button, GUI_TRIGGER_BUTTON_PRESS, ComboboxWidgetPressMain,  NULL);

    Transform2DSetPosition(&combobox->list, 0, combobox->size_y * 2);
    combobox->show = false;

}

void ComboboxWidgetAddItem(EWidgetCombobox *combobox, const char* text){
    EWidgetButton *butt = ListWidgetAddItem(&combobox->list, text);

    WidgetConnect(butt, GUI_TRIGGER_BUTTON_PRESS, ComboboxWidgetPressSub,  combobox->list.size - 1);

    Transform2DSetScale(&combobox->list, combobox->size_x, (combobox->list.size + 1) * combobox->size_y);
}
