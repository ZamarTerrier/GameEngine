#ifndef E_WIDGET_BUTTON_H
#define E_WIDGET_BUTTON_H

#include "engine_includes.h"

#include "e_widget.h"
#include "e_widget_text.h"

#include <wchar.h>

typedef struct{
    EWidget widget;
    EWidgetText text;
    vec4 selfColor;
} EWidgetButton;

void ButtonWidgetInit(EWidgetButton *button, const char *text, vec4 color, EWidget *parent);
void ButtonWidgetSetText(EWidgetButton *button, const char *text);

#endif // E_WIDGET_BUTTON_H
