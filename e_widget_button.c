#include "e_widget_button.h"

void ButtonWidgetPress(EWidget *widget, void* entry, void *arg){
    widget->color = (vec3){0.3, 0.3, 0.3};
}

void ButtonWidgetRelease(EWidget *widget, void* entry, void *arg){
    widget->color = (vec3){0.7, 0.7, 0.7};

    WidgetConfirmTrigger(widget, GUI_TRIGGER_BUTTON_PRESS, NULL);
}

void ButtonWidgetInit(EWidgetButton *button, char *text, EWidget *parent){

    DrawParam dParam = {};

    WidgetInit(button, dParam, parent);
    button->widget.color = (vec3){0.7f, 0.7f, 0.7f};

    TextWidgetInit(&button->text, 9, dParam, &button->widget);
    TextWidgetSetText(&button->text, text);
    vec2 position = { 0, 9 * 2.0f};
    Transform2DSetPosition(&button->text, position);

    WidgetConnect(&button->widget, GUI_TRIGGER_MOUSE_PRESS, ButtonWidgetPress, NULL);
    WidgetConnect(&button->widget, GUI_TRIGGER_MOUSE_RELEASE, ButtonWidgetRelease, NULL);

}

void ButtonWidgetSetText(EWidgetButton *button, char *text){
    TextWidgetSetText(&button->text, text);
}
