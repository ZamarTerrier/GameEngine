#ifndef E_WIDGET_BUTTON_H
#define E_WIDGET_BUTTON_H

#include "stdinclude.h"

#include "e_widget.h"
#include "e_widget_text.h"

typedef struct{
    EWidget widget;
    EWidgetText text;
    vec4 selfColor;
} EWidgetButton;

void ButtonWidgetInit(EWidgetButton *button, char *text, vec4 color, EWidget *parent);
void ButtonWidgetSetText(EWidgetButton *button, char *text);

#endif // E_WIDGET_BUTTON_H
