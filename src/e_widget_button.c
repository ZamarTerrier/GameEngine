#include "e_widget_button.h"


void ButtonWidgetPress(EWidget *widget, void* entry, void *arg){
    EWidgetButton *button = (EWidgetButton *)widget;

    button->widget.color = (vec4){ button->selfColor.x - 0.2f, button->selfColor.y - 0.2f, button->selfColor.z - 0.2f, button->selfColor.w };
}

void ButtonWidgetRelease(EWidget *widget, void* entry, void *arg){

    EWidgetButton *button = (EWidgetButton *)widget;

    button->widget.color = button->selfColor;

    WidgetConfirmTrigger(widget, GUI_TRIGGER_BUTTON_PRESS, NULL);
}

void ButtonWidgetInit(EWidgetButton *button, const char *text, EWidget *parent){

    WidgetInit(button, NULL, parent);
    memcpy(button->widget.go.name, "Button", 6);

    button->widget.type = GUI_TYPE_BUTTON;
    button->selfColor = button->widget.color = (vec4){ 1, 1, 1, 1};

    TextWidgetInit(&button->text, 9, NULL, &button->widget);
    TextWidgetSetText(&button->text, text);

    Transform2DSetPosition(&button->text, 0, 9 * 4.0f);

    WidgetConnect(&button->widget, GUI_TRIGGER_MOUSE_PRESS, ButtonWidgetPress, NULL);
    WidgetConnect(&button->widget, GUI_TRIGGER_MOUSE_RELEASE, ButtonWidgetRelease, NULL);

}

void ButtonWidgetSetText(EWidgetButton *button, const char *text){
    TextWidgetSetText(&button->text, text);
}

void ButtonWidgetSetColor(EWidgetButton *button, vec4 color){
    button->selfColor = button->widget.color = color;
}
