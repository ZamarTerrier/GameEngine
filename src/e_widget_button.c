#include "e_widget_button.h"


int ButtonWidgetPress(EWidget *widget, void* entry, void *arg){
    EWidgetButton *button = (EWidgetButton *)widget;

    button->widget.color.x = button->selfColor.x - 0.2f;
    button->widget.color.y = button->selfColor.y - 0.2f;
    button->widget.color.z = button->selfColor.z - 0.2f;
    button->widget.color.w = button->selfColor.w;

    return 0;
}

int ButtonWidgetRelease(EWidget *widget, void* entry, void *arg){

    EWidgetButton *button = (EWidgetButton *)widget;

    button->widget.color = button->selfColor;

    WidgetConfirmTrigger(widget, ENGINE_WIDGET_TRIGGER_BUTTON_PRESS, NULL);

    return 0;
}

void ButtonWidgetInit(EWidgetButton *button, const char *text, DrawParam *dParam, EWidget *parent){

    WidgetInit(button, dParam, parent);
    WidgetAddDefault(button, dParam->render);
    GameObject2DInitDraw(button);

    memcpy(button->widget.go.name, "Button", 6);
    button->widget.type = ENGINE_WIDGET_TYPE_BUTTON;

    button->selfColor = button->widget.color = (vec4){ 1, 1, 1, 1};

    TextWidgetInit(&button->text, 9, dParam, &button->widget);
    TextWidgetAddDefault(&button->text, dParam->render);
    GameObject2DInitDraw(&button->text);
    TextWidgetSetText(&button->text, text);

    Transform2DSetPosition(&button->text, 0, 9 * 4.0f);

    WidgetConnect(&button->widget, ENGINE_WIDGET_TRIGGER_MOUSE_PRESS, ButtonWidgetPress, NULL);
    WidgetConnect(&button->widget, ENGINE_WIDGET_TRIGGER_MOUSE_RELEASE, ButtonWidgetRelease, NULL);

}

void ButtonWidgetSetImage(EWidgetButton *button, char *path, DrawParam *dParam)
{
    ImageWidgetInitDefault(&button->image, path, dParam, button);

    Transform2DSetScale(&button->image, 64, 64);
    Transform2DSetPosition(&button->image, 0, 0);

    Transform2DSetPosition(&button->text, 9 * 6.0f, 9 * 4.0f);
}

void ButtonWidgetSetText(EWidgetButton *button, const char *text){
    TextWidgetSetText(&button->text, text);
}

void ButtonWidgetSetColor(EWidgetButton *button, float r, float g, float b){
    button->selfColor.x = button->widget.color.x = r;
    button->selfColor.y = button->widget.color.y = g;
    button->selfColor.z = button->widget.color.z = b;
}
