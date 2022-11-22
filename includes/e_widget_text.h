#ifndef E_WIDGET_TEXT_H
#define E_WIDGET_TEXT_H

#include "engine_includes.h"

#include "textObject.h"
#include "gameObject2D.h"

#include "e_widget.h"

typedef struct{
    EWidget widget;
    TextData tData;
} EWidgetText;

void TextWidgetInit(EWidgetText *wt, int fontSize, DrawParam *dParam, EWidget* parent);
void TextWidgetSetColor(EWidgetText* wt, vec3 color);
void TextWidgetSetText(EWidgetText* wt, const char* text);

#endif // E_WIDGET_TEXT_H
