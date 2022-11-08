#include "e_widget_button.h"

void ButtonWidgetPress(EWidget *widget, void* entry, void *arg){
    EWidgetButton *button = (EWidgetButton *)widget;

    button->widget.color = (vec4){ button->selfColor.x - 0.1f, button->selfColor.y - 0.1f, button->selfColor.z - 0.1f, button->selfColor.w };
}

void ButtonWidgetRelease(EWidget *widget, void* entry, void *arg){

    EWidgetButton *button = (EWidgetButton *)widget;

    button->widget.color = button->selfColor;

    WidgetConfirmTrigger(widget, GUI_TRIGGER_BUTTON_PRESS, NULL);
}

void ButtonWidgetInit(EWidgetButton *button, char *text, vec4 color, EWidget *parent){

    DrawParam dParam = {};

    WidgetInit(button, dParam, parent);

    button->selfColor = button->widget.color = color;

    TextWidgetInit(&button->text, 9, dParam, &button->widget);
    TextWidgetSetText(&button->text, text);

    Transform2DSetPosition(&button->text, 0, 9 * 2.0f);

    WidgetConnect(&button->widget, GUI_TRIGGER_MOUSE_PRESS, ButtonWidgetPress, NULL);
    WidgetConnect(&button->widget, GUI_TRIGGER_MOUSE_RELEASE, ButtonWidgetRelease, NULL);

}

void ButtonWidgetSetText(EWidgetButton *button, char *text){
    TextWidgetSetText(&button->text, text);
}
