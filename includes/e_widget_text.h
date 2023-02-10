#ifndef E_WIDGET_TEXT_H
#define E_WIDGET_TEXT_H

#include "engine_includes.h"

#include "textObject.h"
#include "gameObject2D.h"

#include "e_widget.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct{
    EWidget widget;
    TextData tData;
} EWidgetText;

void TextWidgetInit(EWidgetText *wt, int fontSize, DrawParam *dParam, EWidget* parent);
void TextWidgetSetColor(EWidgetText* wt, vec3 color);
void TextWidgetSetText(EWidgetText* wt, const char* text);
void TextWidgetGetText(EWidgetText* wt, char* dest);

#ifdef __cplusplus
}
#endif

#endif // E_WIDGET_TEXT_H
